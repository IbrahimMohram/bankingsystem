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

using namespace std;

static int callback(void *data, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

Database::Database() : db(nullptr) {
	initDB();
}

Database::~Database() {
}

void Database::initDB() {

	int rc;
	rc = sqlite3_open(DBNAME, &db);

	if(rc) {
		cerr << "Error opening the future bank database: " << sqlite3_errmsg(db) << endl;
	}
}

bool Database::createAccountsTable() {
	char *zErrMsg = nullptr;
	int rc;
	string sql;

	sql = "CREATE TABLE IF NOT EXISTS ACCOUNTS(" \
			"ID 			 INT    PRIMARY KEY NOT NULL,"   \
			"LOCKED		 BOOLEAN    NOT NULL," \
			"BALANCE         INT    NOT NULL);";

	rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		cerr <<  "SQL error: " << zErrMsg << endl;
		sqlite3_free(zErrMsg);
		return false;
	}
	else
		return true;

	return false;
}

bool Database::createPersonsTable() {

	char *zErrMsg = nullptr;
	int rc;
	string sql;

	sql = "CREATE TABLE IF NOT EXISTS PERSONS(" \
	      "ID               INT     NOT NULL," \
	      "USERNAME         TEXT    NOT NULL UNIQUE," \
	      "FIRSTNAME        TEXT    NOT NULL," \
	      "LASTNAME         TEXT    NOT NULL," \
	      "NATIONALID       TEXT    NOT NULL," \
	      "PASSWORD         TEXT    NOT NULL," \
	      "TYPE             INT     NOT NULL,"  \
	      "LOCKED           BOOLEAN NOT NULL," \
	      "CAPS             INT     NOT NULL,"
	      "PRIMARY KEY (ID));";

	rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		cerr <<  "SQL error: " << zErrMsg << endl;
		sqlite3_free(zErrMsg);
		return false;
	}
	else
		return true;

	return false;
}

bool Database::insertAccount(Account *acct) {

	const char *zErrMsg;
	int rc;
	string sql;
	sqlite3_stmt *stmt;

	sql = "INSERT OR REPLACE INTO ACCOUNTS VALUES (?,?,?);";

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

	rc = sqlite3_bind_int64(stmt, 3, acct->getBalance() );
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

	sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

	return true;
}

bool Database::deleteAccount(Account *acct) {

	const char *zErrMsg;
	int rc;
	string sql;
	sqlite3_stmt *stmt;

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

	sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

	delete acct;

	return true;
}

Account* Database::retrieveAccount(const int account_id) {
	return nullptr;
}

bool Database::insertPerson(Person *p) {

	const char *zErrMsg;
	int rc;
	string sql;
	sqlite3_stmt *stmt;

	sql = "INSERT OR REPLACE INTO PERSONS VALUES (?,?,?,?,?,?,?,?,?);";

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	assert(rc == SQLITE_OK);

	rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, &zErrMsg);
	if(SQLITE_OK != rc) {
		cerr << "Can't prepare insert statment " << sql.c_str() << " "  << rc << " " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		return false;
	}


	rc = sqlite3_bind_int64(stmt, 1, p->getId() );
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

	rc = sqlite3_bind_int64(stmt, 7, p->getUserType() );
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

	rc = sqlite3_bind_int64(stmt, 9, p->getCaps() );
	if(SQLITE_OK != rc) {
		fprintf(stderr, "Error binding value in insert (%i): %s\n", rc, sqlite3_errmsg(db));
		sqlite3_close(db);
		return false;
	}


	rc = sqlite3_step(stmt);
	if(SQLITE_DONE != rc) {
		cerr <<  "Error binding value in insert " << rc << " " <<  sqlite3_errmsg(db) << endl;
		return false;
	}

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

	sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

	delete p;

	return true;
}

Person* Database::retrievePerson(const string username) { // TODO: Setting user capabilities should be set here

	return nullptr;
}

