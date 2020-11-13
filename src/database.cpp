/*
 * database.cpp
 *
 *  Created on: Nov 6, 2020
 *      Author: Amira
 */

#include <iostream>
#include <string>
#include <string.h>
#include "database.h"
#include "session.h"
#include <assert.h>

#define USERID 0
#define USERNAME 1
#define FIRSTNAME 2
#define LASTNAME 3
#define NATIONALID 4
#define PASSWORD 5
#define USERTYPE 6
#define USERLOCK 7
#define USERCAPS 8

using namespace std;

Database::Database() : db(nullptr) {
	if (initDB()) {
		if (!createPersonsTable()) {
			cerr << "Error creating the persons table" << endl;
			exit(-2);
		}

		if (!createAccountsTable()) {
			cerr << "Error creating the accounts table" << endl;
			exit(-2);
		}
	}
	else {
		cerr << "Error connecting to the bank database" << endl;
		exit(-1);
	}
}

Database::~Database() {
	sqlite3_close(db);
}

bool Database::initDB() {

	int rc;
	rc = sqlite3_open(DBNAME, &db);

	if(rc) {
		cerr << "Error opening the future bank database: " << sqlite3_errmsg(db) << endl;
		return false;
	}

	rc = sqlite3_busy_timeout(db, 0);
	if(rc)
		cerr << "Error setting busy handler for the future bank database: " << sqlite3_errmsg(db) << endl;

	return true;
}

bool Database::createAccountsTable() {
	char *zErrMsg = nullptr;
	int rc;
	string sql;

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	assert(rc == SQLITE_OK);

	sql = "CREATE TABLE IF NOT EXISTS ACCOUNTS(" \
			"ID 			 INT        PRIMARY KEY NOT NULL,"   \
			"LOCKED		     BOOLEAN    NOT NULL," \
			"OWNER			 INT		NOT NULL," \
			"BALANCE         INT        NOT NULL," \
			"FOREIGN KEY(OWNER) REFERENCES PERSONS(ID));";

	rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		cerr <<  "SQL error: " << zErrMsg << endl;
		sqlite3_free(zErrMsg);
		return false;
	}
	else {
	    sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);
		return true;
	}

	return false;
}

bool Database::createPersonsTable() {

	char *zErrMsg = nullptr;
	int rc;
	string sql;

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	assert(rc == SQLITE_OK);

	sql = "CREATE TABLE IF NOT EXISTS PERSONS(" \
	      "ID               INT     NOT NULL," \
	      "USERNAME         TEXT    NOT NULL UNIQUE," \
	      "FIRSTNAME        TEXT    NOT NULL," \
	      "LASTNAME         TEXT    NOT NULL," \
	      "NATIONALID       TEXT    NOT NULL," \
	      "PASSWORD         TEXT    NOT NULL," \
	      "TYPE             INT     NOT NULL,"  \
	      "LOCKED           INT     NOT NULL," \
	      "CAPS             INT     NOT NULL,"
	      "PRIMARY KEY (ID));";

	rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		cerr <<  "SQL error: " << zErrMsg << endl;
		sqlite3_free(zErrMsg);
		return false;
	}
	else {
	    sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);
		return true;
	}

	return false;
}

bool Database::insertAccount(Account *acct) {

	const char *zErrMsg;
	int rc;
	string sql;
	sqlite3_stmt *stmt;

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	assert(rc == SQLITE_OK);

	sql = "INSERT OR REPLACE INTO ACCOUNTS VALUES (?,?,?,?);";

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	assert(rc == SQLITE_OK);

	rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, &zErrMsg);
	if(SQLITE_OK != rc) {
		cerr << "Can't prepare insert statment " << sql.c_str() << " "  << rc << " " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_bind_int64(stmt, 1, acct->getId());
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in insert " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_bind_int64(stmt, 2, acct->isLocked() );
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in insert " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_bind_int64(stmt, 3, acct->getCustomerId());
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in insert " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_bind_int64(stmt, 4, acct->getBalance() );
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in insert " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_step(stmt);
	if(SQLITE_DONE != rc) {
		cerr <<  "Error binding value in insert " << rc << " " <<  sqlite3_errmsg(db) << endl;
		return false;
	}
	sqlite3_finalize(stmt);
	sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

	return true;
}

bool Database::deleteAccount(Account *acct) {

	const char *zErrMsg;
	int rc;
	string sql;
	sqlite3_stmt *stmt;

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	assert(rc == SQLITE_OK);

	sql = "DELETE FROM ACCOUNTS WHERE ID = ?;";

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	assert(rc == SQLITE_OK);

	rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, &zErrMsg);
	if (SQLITE_OK != rc) {
		cerr << "Can't prepare delete statment " << sql.c_str() << " " << rc
				<< " " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_bind_int64(stmt, 1, acct->getId() );
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in delete " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_step(stmt);
	if(SQLITE_DONE != rc) {
		cerr <<  "Error binding value in delete " << rc << " " <<  sqlite3_errmsg(db) << endl;
		return false;
	}

	sqlite3_finalize(stmt);
	sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

	delete acct;


	return true;
}

Account* Database::retrieveAccount(const int account_id) {

	const char *zErrMsg = nullptr;
	int rc;
	string sql;
	sqlite3_stmt *stmt = nullptr;
	int accountid = 0;
	int lockstatus = 0;
	int custid = 0;
	int balance = 0;

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	assert(rc == SQLITE_OK);

	sql = "SELECT * from ACCOUNTS WHERE ID = ?";
	rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, &zErrMsg);
	if(SQLITE_OK != rc) {
		cerr << "Can't prepare select statment " << sql.c_str() << " "  << rc << " " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return nullptr;
	}

	rc = sqlite3_bind_int64(stmt, 1, account_id);
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in select " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return nullptr;
	}

	int step = sqlite3_step(stmt);
	if (step == SQLITE_ROW) {
		accountid = sqlite3_column_int(stmt, 0);
		lockstatus = sqlite3_column_int(stmt, 1);
		custid = sqlite3_column_int(stmt, 2);
		balance = sqlite3_column_int(stmt, 3);
	}

	Account *acct = new Account();
	acct->setId(accountid);
	acct->setBalance(balance);
	acct->setCustomerId(custid);
	lockstatus ? acct->lock() : acct->unlock();

	sqlite3_finalize(stmt);
    sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

	return acct;
}

Account* Database::retrieveAccountByCustomer(const int customer_id) {

	const char *zErrMsg = nullptr;
	int rc;
	string sql;
	sqlite3_stmt *stmt = nullptr;
	int accountid = 0;
	int lockstatus = 0;
	int custid = 0;
	int balance = 0;

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	assert(rc == SQLITE_OK);

	sql = "SELECT * from ACCOUNTS WHERE OWNER = ?";
	rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, &zErrMsg);
	if(SQLITE_OK != rc) {
		cerr << "Can't prepare select statment " << sql.c_str() << " "  << rc << " " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return nullptr;
	}

	rc = sqlite3_bind_int64(stmt, 1, customer_id);
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in select " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return nullptr;
	}

	int step = sqlite3_step(stmt);
	if (step == SQLITE_ROW) {
		accountid = sqlite3_column_int(stmt, 0);
		lockstatus = sqlite3_column_int(stmt, 1);
		custid = sqlite3_column_int(stmt, 2);
		balance = sqlite3_column_int(stmt, 3);
	}

	Account *acct = new Account();
	acct->setId(accountid);
	acct->setBalance(balance);
	acct->setCustomerId(custid);
	lockstatus ? acct->lock() : acct->unlock();

	sqlite3_finalize(stmt);
    sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

	return acct;
}

bool Database::insertPerson(Person *p) {

	const char *zErrMsg;
	int rc;
	string sql;
	sqlite3_stmt *stmt;

	sql = "INSERT OR REPLACE INTO PERSONS VALUES (?,?,?,?,?,?,?,?,?);";

	//rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	//assert(rc == SQLITE_OK);

	rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, &zErrMsg);
	if(SQLITE_OK != rc) {
		cerr << "Can't prepare insert statment " << sql.c_str() << " "  << rc << " " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}


	rc = sqlite3_bind_int64(stmt, 1, p->getId());
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in insert " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}


	rc = sqlite3_bind_text(stmt, 2, p->getUserName().c_str(), p->getUserName().length(), SQLITE_TRANSIENT);
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in insert " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_bind_text(stmt, 3, p->getFirstName().c_str(), p->getFirstName().length(), SQLITE_TRANSIENT);
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in insert " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_bind_text(stmt, 4, p->getLastName().c_str(), p->getLastName().length(), SQLITE_TRANSIENT);
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in insert " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_bind_text(stmt, 5, p->getNationalId().c_str(), p->getNationalId().length(), SQLITE_TRANSIENT);
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in insert " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_bind_text(stmt, 6, p->getPassword().c_str(), p->getPassword().length(), SQLITE_TRANSIENT);
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in insert " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_bind_int64(stmt, 7, setUserType(p) );
	if(SQLITE_OK != rc) {
		fprintf(stderr, "Error binding value in insert (%i): %s\n", rc, sqlite3_errmsg(db));
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_bind_int64(stmt, 8, p->isLocked() );
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in insert " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_bind_int64(stmt, 9, computeUserCaps(p) );
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in insert " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_step(stmt);
	if(SQLITE_DONE != rc) {
		cerr <<  "Error binding value in insert " << rc << " " <<  sqlite3_errmsg(db) << endl;
		return false;
	}

	sqlite3_finalize(stmt);
	sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

	return true;
}

bool Database::deletePerson(Person *p) {

	const char *zErrMsg;
	int rc;
	string sql;
	sqlite3_stmt *stmt;

	sql = "DELETE FROM PERSONS WHERE USERNAME = ?;";

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	assert(rc == SQLITE_OK);

	rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, &zErrMsg);
	if (SQLITE_OK != rc) {
		cerr << "Can't prepare delete statment " << sql.c_str() << " " << rc
				<< " " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}


	rc = sqlite3_bind_text(stmt, 1, p->getUserName().c_str(), p->getUserName().length(), SQLITE_TRANSIENT);
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in delete " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_step(stmt);
	if(SQLITE_DONE != rc) {
		cerr <<  "Error binding value in delete " << rc << " " <<  sqlite3_errmsg(db) << endl;
		return false;
	}

	sqlite3_finalize(stmt);
	sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

	delete p;

	return true;
}

int Database::setUserType(Person *p) {
	if (typeid(*p) == typeid(Customer))
		return Session::CUSTOMER;
	else if (typeid(*p) == typeid(Employee))
		return Session::EMPLOYEE;
	else if (typeid(*p) == typeid(Admin))
		return Session::ADMIN;
	else
		return Session::UNKNOWN;
}

Person* Database::retrievePerson(const string username) const {

	const char *zErrMsg = nullptr;
	int rc;
	string sql;
	sqlite3_stmt *stmt = nullptr;
	int userid = 0;
	string uname = "";
	string fname = "";
	string lname = "";
	string natid = "";
	string password = "";
	int usertype = 0;
	int lockstatus = 0;
    int caps = 0;

	//rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	//assert(rc == SQLITE_OK);

	sql = "SELECT * from PERSONS WHERE USERNAME = ?";
	rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, &zErrMsg);
	if(SQLITE_OK != rc) {
		cerr << "Can't prepare select statment " << sql.c_str() << " "  << rc << " " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return nullptr;
	}

	rc = sqlite3_bind_text(stmt, 1, username.c_str(), username.length(), SQLITE_TRANSIENT);
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in select " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return nullptr;
	}

	int step = sqlite3_step(stmt);
	if (step == SQLITE_ROW) {
		userid = sqlite3_column_int(stmt, USERID);
		uname = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, USERNAME)));
		fname = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, FIRSTNAME)));
		lname = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, LASTNAME)));
		natid = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, NATIONALID)));
		password = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, PASSWORD)));
		usertype = sqlite3_column_int(stmt, USERTYPE);
		lockstatus = sqlite3_column_int(stmt, USERLOCK);
		caps = sqlite3_column_int(stmt, USERCAPS);
	}

	if (usertype == Session::CUSTOMER) {
		Customer *person = new Customer();
		person->setId(userid);
		person->setUserName(uname);
		person->setFirstName(fname);
		person->setLastName(lname);
		person->setNationalId(natid);
		person->setPassword(password);
		person->setUserType(usertype);
		lockstatus ? person->lock() : person->unlock();
		person->setCaps(caps);
		sqlite3_finalize(stmt);
		return person;
	}

	else if (usertype == Session::EMPLOYEE) {
		Employee *person = new Employee();
		person->setId(userid);
		person->setUserName(uname);
		person->setFirstName(fname);
		person->setLastName(lname);
		person->setNationalId(natid);
		person->setPassword(password);
		person->setUserType(usertype);
		lockstatus ? person->lock() : person->unlock();
		person->setCaps(caps);
		person->cap_acctCreate(caps & Session::ACCOUNT_CREATE);
		person->cap_acctUpdate(caps & Session::ACCOUNT_UPDATE);
		person->cap_acctDelete(caps & Session::ACCOUNT_DELETE);
		person->cap_acctActivate(caps & Session::ACCOUNT_ACTIVATE);
		person->cap_acctDeactivate(caps & Session::ACCOUNT_DEACTIVATE);
		person->cap_acctPrintInfo(caps & Session::ACCOUNT_PRINT_INFO);
		person->cap_acctListAll(caps & Session::ACCOUNT_LIST_ALL);
		person->cap_custCreate(caps & Session::CUSTOMER_CREATE);
		person->cap_custUpdate(caps & Session::CUSTOMER_UPDATE);
		person->cap_custDelete(caps & Session::CUSTOMER_DELETE);
		person->cap_custActivate(caps & Session::CUSTOMER_ACTIVATE);
		person->cap_custDeactivate(caps & Session::CUSTOMER_DEACTIVATE);
		person->cap_custPrintInfo(caps & Session::CUSTOMER_PRINT_INFO);
		person->cap_custListAll(caps & Session::CUSTOMER_LIST_ALL);
		sqlite3_finalize(stmt);
		return person;
	}

	else if (usertype == Session::ADMIN) {
		Admin *person = new Admin();
		person->setId(userid);
		person->setUserName(uname);
		person->setFirstName(fname);
		person->setLastName(lname);
		person->setNationalId(natid);
		person->setPassword(password);
		person->setUserType(usertype);
		lockstatus ? person->lock() : person->unlock();
		person->setCaps(caps);
		person->cap_acctCreate(caps & Session::ACCOUNT_CREATE);
		person->cap_acctUpdate(caps & Session::ACCOUNT_UPDATE);
		person->cap_acctDelete(caps & Session::ACCOUNT_DELETE);
		person->cap_acctActivate(caps & Session::ACCOUNT_ACTIVATE);
		person->cap_acctDeactivate(caps & Session::ACCOUNT_DEACTIVATE);
		person->cap_acctPrintInfo(caps & Session::ACCOUNT_PRINT_INFO);
		person->cap_acctListAll(caps & Session::ACCOUNT_LIST_ALL);

		person->cap_custCreate(caps & Session::CUSTOMER_CREATE);
		person->cap_custUpdate(caps & Session::CUSTOMER_UPDATE);
		person->cap_custDelete(caps & Session::CUSTOMER_DELETE);
		person->cap_custActivate(caps & Session::CUSTOMER_ACTIVATE);
		person->cap_custDeactivate(caps & Session::CUSTOMER_DEACTIVATE);
		person->cap_custPrintInfo(caps & Session::CUSTOMER_PRINT_INFO);
		person->cap_custListAll(caps & Session::CUSTOMER_LIST_ALL);

		person->cap_AdminCreate(caps & Session::ADMIN_CREATE);
		person->cap_AdminUpdate(caps & Session::ADMIN_UPDATE);
		person->cap_AdminDelete(caps & Session::ADMIN_DELETE);
		person->cap_AdminActivate(caps & Session::ADMIN_ACTIVATE);
		person->cap_AdminDeactivate(caps & Session::ADMIN_DEACTIVATE);
		person->cap_AdminPrintInfo(caps & Session::ADMIN_PRINT_INFO);
		person->cap_AdminListAll(caps & Session::ADMIN_LIST_ALL);
		sqlite3_finalize(stmt);
		return person;
	}

	return nullptr;
}

int Database::computeUserCaps(Person *p) {

	int usertype = setUserType(p);
	vector<int> allcaps;
	int usercaps = 0;

	switch (usertype) {
	case Session::CUSTOMER:
		allcaps.push_back(Session::CUSTOMER_PRINT_OWN_INFO);
		allcaps.push_back(Session::CUSTOMER_TRANSFER_TO_ACCOUNT);
		allcaps.push_back(Session::ACCOUNT_PRINT_OWN_INFO);
		break;

	case Session::EMPLOYEE: {
		Employee *emp = dynamic_cast<Employee*>(p);
		if (emp->canCreateAccount()) {
			allcaps.push_back(Session::ACCOUNT_CREATE);
		}
		if (emp->canUpdateAccount()) {
			allcaps.push_back(Session::ACCOUNT_UPDATE);
		}
		if (emp->canDeleteAccount()) {
			allcaps.push_back(Session::ACCOUNT_DELETE);
		}
		if (emp->canActivateAccount()) {
			allcaps.push_back(Session::ACCOUNT_ACTIVATE);
		}
		if (emp->canDeactivateAccount()) {
			allcaps.push_back(Session::ACCOUNT_DEACTIVATE);
		}
		if (emp->canListAllAccounts()) {
			allcaps.push_back(Session::ACCOUNT_LIST_ALL);
		}
		if (emp->canPrintAccountInfo()) {
			allcaps.push_back(Session::ACCOUNT_PRINT_INFO);
		}
		if (emp->canCreateCustomer()) {
			allcaps.push_back(Session::CUSTOMER_CREATE);
		}
		if (emp->canUpdateCustomer()) {
			allcaps.push_back(Session::CUSTOMER_UPDATE);
		}
		if (emp->canDeleteCustomer()) {
			allcaps.push_back(Session::CUSTOMER_DELETE);
		}
		if (emp->canActivateCustomer()) {
			allcaps.push_back(Session::CUSTOMER_ACTIVATE);
		}
		if (emp->canDeactivateCustomer()) {
			allcaps.push_back(Session::CUSTOMER_DEACTIVATE);
		}
		if (emp->canListAllCustomers()) {
			allcaps.push_back(Session::CUSTOMER_LIST_ALL);
		}
		if (emp->canPrintCustomerInfo()) {
			allcaps.push_back(Session::CUSTOMER_PRINT_INFO);
		}
		break;
	}
	case Session::ADMIN: {
		Admin *adm = dynamic_cast<Admin*>(p);
		if (adm->canCreateAdmin()) {
			allcaps.push_back(Session::ADMIN_CREATE);
		}
		if (adm->canUpdateAdmin()) {
			allcaps.push_back(Session::ADMIN_UPDATE);
		}
		if (adm->canDeleteAdmin()) {
			allcaps.push_back(Session::ADMIN_DELETE);
		}
		if (adm->canActivateAdmin()) {
			allcaps.push_back(Session::ADMIN_ACTIVATE);
		}
		if (adm->canDeactivateAdmin()) {
			allcaps.push_back(Session::ADMIN_DEACTIVATE);
		}
		if (adm->canPrintAdminInfo()) {
			allcaps.push_back(Session::ADMIN_PRINT_INFO);
		}
		if (adm->canListAllAdmin()) {
			allcaps.push_back(Session::ADMIN_LIST_ALL);
		}
		if (adm->canCreateEmployee()) {
			allcaps.push_back(Session::EMPLOYEE_CREATE);
		}
		if (adm->canUpdateEmployee()) {
			allcaps.push_back(Session::EMPLOYEE_UPDATE);
		}
		if (adm->canDeleteEmployee()) {
			allcaps.push_back(Session::EMPLOYEE_DELETE);
		}
		if (adm->canActivateEmployee()) {
			allcaps.push_back(Session::EMPLOYEE_ACTIVATE);
		}
		if (adm->canDeactivateEmployee()) {
			allcaps.push_back(Session::EMPLOYEE_DEACTIVATE);
		}
		if (adm->canPrintEmployeeInfo()) {
			allcaps.push_back(Session::EMPLOYEE_PRINT_INFO);
		}
		if (adm->canListAllEmployee()) {
			allcaps.push_back(Session::EMPLOYEE_LIST_ALL);
		}
		if (adm->canCreateAccount()) {
			allcaps.push_back(Session::ACCOUNT_CREATE);
		}
		if (adm->canUpdateAccount()) {
			allcaps.push_back(Session::ACCOUNT_UPDATE);
		}
		if (adm->canDeleteAccount()) {
			allcaps.push_back(Session::ACCOUNT_DELETE);
		}
		if (adm->canActivateAccount()) {
			allcaps.push_back(Session::ACCOUNT_ACTIVATE);
		}
		if (adm->canDeactivateAccount()) {
			allcaps.push_back(Session::ACCOUNT_DEACTIVATE);
		}
		if (adm->canListAllAccounts()) {
			allcaps.push_back(Session::ACCOUNT_LIST_ALL);
		}
		if (adm->canPrintAccountInfo()) {
			allcaps.push_back(Session::ACCOUNT_PRINT_INFO);
		}
		if (adm->canCreateCustomer()) {
			allcaps.push_back(Session::CUSTOMER_CREATE);
		}
		if (adm->canUpdateCustomer()) {
			allcaps.push_back(Session::CUSTOMER_UPDATE);
		}
		if (adm->canDeleteCustomer()) {
			allcaps.push_back(Session::CUSTOMER_DELETE);
		}
		if (adm->canActivateCustomer()) {
			allcaps.push_back(Session::CUSTOMER_ACTIVATE);
		}
		if (adm->canDeactivateCustomer()) {
			allcaps.push_back(Session::CUSTOMER_DEACTIVATE);
		}
		if (adm->canListAllCustomers()) {
			allcaps.push_back(Session::CUSTOMER_LIST_ALL);
		}
		if (adm->canPrintCustomerInfo()) {
			allcaps.push_back(Session::CUSTOMER_PRINT_INFO);
		}
		break;
	}
	default:
		break;
	}

	for (vector<int>::iterator it = allcaps.begin() ; it != allcaps.end() ; ++it)
		usercaps |= *it;

	return usercaps;
}

int Database::generateAccountNumber() {

	const char *zErrMsg = nullptr;
	int rc;
	string sql;
	sqlite3_stmt *stmt = nullptr;
	int maxid = 0;

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	assert(rc == SQLITE_OK);

	sql = "SELECT MAX(ID) from ACCOUNTS;";
	rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, &zErrMsg);
	if (SQLITE_OK != rc) {
		cerr << "Can't prepare select statment " << sql.c_str() << " " << rc
				<< " " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		cerr << "Couldn't generate a valid account number, please contact your administrator!!!" << endl;
		exit(-1);
	}

	int step = sqlite3_step(stmt);
	if (step == SQLITE_ROW) {
		maxid = sqlite3_column_int(stmt, 0);
		if (maxid <= 0)
			return 1;
	}

	sqlite3_finalize(stmt);
    sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

	return maxid+1;
}

int Database::generatePersonNumber() {

	const char *zErrMsg = nullptr;
	int rc;
	string sql;
	sqlite3_stmt *stmt = nullptr;
	int maxid = 0;

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	assert(rc == SQLITE_OK);

	sql = "SELECT MAX(ID) from PERSONS;";
	rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, &zErrMsg);
	if (SQLITE_OK != rc) {
		cerr << "Can't prepare select statment " << sql.c_str() << " " << rc
				<< " " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		cerr << "Couldn't generate a valid person number, please contact your administrator!!!" << endl;
		exit(-1);
	}

	int step = sqlite3_step(stmt);
	if (step == SQLITE_ROW) {
		maxid = sqlite3_column_int(stmt, 0);
		if (maxid <= 0)
			return 1;
	}

	sqlite3_finalize(stmt);
    sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

	return maxid+1;
}

vector<Person*> Database::getAllPersons(const int person_type) {

	const char *zErrMsg = nullptr;
	int rc;
	string sql;
	sqlite3_stmt *stmt = nullptr;
	int userid = 0;
	string uname = "";
	string fname = "";
	string lname = "";
	string natid = "";
	string password = "";
	int usertype = 0;
	int lockstatus = 0;
    int caps = 0;
    vector<Person*> list;

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	assert(rc == SQLITE_OK);

	sql = "SELECT * from PERSONS WHERE TYPE = ?";
	rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, &zErrMsg);
	if(SQLITE_OK != rc) {
		cerr << "Can't prepare select statment " << sql.c_str() << " "  << rc << " " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		exit(-1);
	}

	rc = sqlite3_bind_int64(stmt, 1, person_type);
	if(SQLITE_OK != rc) {
		cerr <<  "Error binding value in select " << rc << " " <<  sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		exit(-1);
	}

	int step;
	for (;;) {
		step = sqlite3_step(stmt);
		if (step == SQLITE_DONE)
			break;
		else if (step == SQLITE_ROW) {
			userid = sqlite3_column_int(stmt, USERID);
			uname = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt,USERNAME)));
			fname = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt,FIRSTNAME)));
			lname = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt,LASTNAME)));
			natid = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt,NATIONALID)));
			password = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt,PASSWORD)));
			usertype = sqlite3_column_int(stmt, USERTYPE);
			lockstatus = sqlite3_column_int(stmt, USERLOCK);
			caps = sqlite3_column_int(stmt, USERCAPS);

			if (usertype == person_type) { // Extra check
				Person *person = new Person();
				person->setId(userid);
				person->setUserName(uname);
				person->setFirstName(fname);
				person->setLastName(lname);
				person->setNationalId(natid);
				person->setPassword(password);
				person->setUserType(usertype);
				lockstatus ? person->lock() : person->unlock();
				person->setCaps(caps);
				list.push_back(person);
			}
		}
		else {
			cerr << "Error retrieving users info from the database" << endl;
			sqlite3_close(db);
			exit(-1);
		}
	}
	sqlite3_finalize(stmt);
    sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

	return list;
}

vector<Account*> Database::getAllAccounts() {

	const char *zErrMsg = nullptr;
	int rc;
	string sql;
	sqlite3_stmt *stmt = nullptr;
	int accountid = 0;
	int lockstatus = 0;
	int custid = 0;
	int balance = 0;
	vector<Account*> list;

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	assert(rc == SQLITE_OK);

	sql = "SELECT * from ACCOUNTS;";
	rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, &zErrMsg);
	if(SQLITE_OK != rc) {
		cerr << "Can't prepare select statment " << sql.c_str() << " "  << rc << " " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		exit(-1);
	}

	int step;
	for (;;) {
		step = sqlite3_step(stmt);
		if (step == SQLITE_DONE)
			break;
		if (step == SQLITE_ROW) {
			accountid = sqlite3_column_int(stmt, 0);
			lockstatus = sqlite3_column_int(stmt, 1);
			custid = sqlite3_column_int(stmt, 2);
			balance = sqlite3_column_int(stmt, 3);

			Account *acct = new Account();
			acct->setId(accountid);
			acct->setBalance(balance);
			acct->setCustomerId(custid);
			lockstatus ? acct->lock() : acct->unlock();

			list.push_back(acct);
		}
		else{
			cerr << "Error retrieving accounts info from the database" << endl;
			sqlite3_close(db);
			exit(-1);
		}
	}

	sqlite3_finalize(stmt);
    sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

	return list;
}

int Database::getUsersCount() {
	const char *zErrMsg = nullptr;
	int rc;
	string sql;
	sqlite3_stmt *stmt = nullptr;
	int total = 0;

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	assert(rc == SQLITE_OK);

	sql = "SELECT COUNT(*) from PERSONS;";
	rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, &zErrMsg);
	if(SQLITE_OK != rc) {
		cerr << "Can't prepare select statment " << sql.c_str() << " "  << rc << " " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		exit(-1);
	}

	int step = sqlite3_step(stmt);
	if (step == SQLITE_ROW) {
		total = sqlite3_column_int(stmt, 0);

	} else {
		cerr << "Error retrieving persons count from the database" << endl;
		sqlite3_close(db);
		exit(-1);
	}

	sqlite3_finalize(stmt);
	sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

	return total;
}

Customer* Database::retrieveCustomerByAccount(const int accountid) const {

	const char *zErrMsg = nullptr;
	int rc;
	string sql;
	sqlite3_stmt *stmt = nullptr;
	int userid = 0;
	string uname = "";
	string fname = "";
	string lname = "";
	string natid = "";
	string password = "";
	int usertype = 0;
	int lockstatus = 0;
	int caps = 0;

	//rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	//assert(rc == SQLITE_OK);

	sql = "SELECT * from PERSONS WHERE ID = (SELECT ID FROM ACCOUNTS WHERE OWNER = ?);";
	rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, &zErrMsg);
	if (SQLITE_OK != rc) {
		cerr << "Can't prepare select statment " << sql.c_str() << " " << rc
				<< " " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return nullptr;
	}

	rc = sqlite3_bind_int64(stmt, 1, accountid);
	if (SQLITE_OK != rc) {
		cerr << "Error binding value in select " << rc << " "
				<< sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return nullptr;
	}

	int step = sqlite3_step(stmt);
	if (step == SQLITE_ROW) {
		userid = sqlite3_column_int(stmt, USERID);
		uname = string(
				reinterpret_cast<const char*>(sqlite3_column_text(stmt,
						USERNAME)));
		fname = string(
				reinterpret_cast<const char*>(sqlite3_column_text(stmt,
						FIRSTNAME)));
		lname = string(
				reinterpret_cast<const char*>(sqlite3_column_text(stmt,
						LASTNAME)));
		natid = string(
				reinterpret_cast<const char*>(sqlite3_column_text(stmt,
						NATIONALID)));
		password = string(
				reinterpret_cast<const char*>(sqlite3_column_text(stmt,
						PASSWORD)));
		usertype = sqlite3_column_int(stmt, USERTYPE);
		lockstatus = sqlite3_column_int(stmt, USERLOCK);
		caps = sqlite3_column_int(stmt, USERCAPS);
	}

	if (usertype == Session::CUSTOMER) {
		Customer *person = new Customer();
		person->setId(userid);
		person->setUserName(uname);
		person->setFirstName(fname);
		person->setLastName(lname);
		person->setNationalId(natid);
		person->setPassword(password);
		person->setUserType(usertype);
		lockstatus ? person->lock() : person->unlock();
		person->setCaps(caps);
		sqlite3_finalize(stmt);
		return person;
	}

	return nullptr;
}
