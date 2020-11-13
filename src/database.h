/*
 * database.h
 *
 *  Created on: Nov 6, 2020
 *      Author: Amira
 */

#ifndef SRC_DATABASE_H_
#define SRC_DATABASE_H_

#include <vector>
#include <map>
#include <sqlite3.h>
#include "account.h"
#include "person.h"
#include "customer.h"
#include "employee.h"
#include "admin.h"


using namespace std;

#define DBNAME "future_bank.db"

class Database {

private:
	sqlite3 *db;
	bool initDB();
	int setUserType(Person *p);
	int computeUserCaps(Person *p);
	bool createAccountsTable();
	bool createPersonsTable();


public:
	Database();
	~Database();

	bool insertAccount(Account *acct);
	bool deleteAccount(Account *acct);
	Account* retrieveAccount(const int account_id);
	Account* retrieveAccountByCustomer(const int customer_id);
	Customer* retrieveCustomerByAccount(const int accountid) const;

	bool insertPerson(Person *p);
	bool deletePerson(Person *p);
	Person* retrievePerson(const string username) const;

	int generateAccountNumber();
	int generatePersonNumber();
	int getUsersCount();

	vector<Person*> getAllPersons(const int person_type);
	vector<Account*> getAllAccounts();

};

#endif /* SRC_DATABASE_H_ */
