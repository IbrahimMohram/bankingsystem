/*
 * employee.cpp
 *
 *  Created on: Nov 3, 2020
 *      Author: amira
 */

#include <assert.h>
#include <typeinfo>
#ifdef __linux__
#include <unistd.h>
#endif
#include "session.h"
#include "userinterface.h"

/*
 * Session methods
 * */

bool Session::createAccount(Account *acct) {
	if (!isAuthorized(Session::ACCOUNT_CREATE))
		return false;

	if (typeid(*acct) != typeid(Account))
		return false;


	if (!acct || m_db->retrieveAccount(acct->getId()))
			return false;

	if (!m_db->insertAccount(acct))
		return false;
	else
		return true;

	return false;
}

bool Session::deleteAccount(Account *acct) {
	if (!isAuthorized(Session::ACCOUNT_DELETE))
		return false;

	if (typeid(*acct) != typeid(Account))
		return false;

	if (!acct || m_db->retrieveAccount(acct->getId()))
		return false;

	if (!m_db->deleteAccount(acct))
		return false;
	else
		return true;

	return false;
}

bool Session::updateAccount(Account *acct) {
	if (!isAuthorized(Session::ACCOUNT_UPDATE))
		return false;

	if (typeid(*acct) != typeid(Account))
		return false;

	if (!acct || !m_db->deleteAccount(acct))
		return false;
	else
		return true;

	return false;
}

bool Session::deactivateAccount(Account *acct) {
	if (!isAuthorized(Session::ACCOUNT_DEACTIVATE))
		return false;


	if (typeid(*acct) != typeid(Account))
		return false;

	if (!acct)
		return false;

	acct->lock();

	if (!m_db->insertAccount(acct))
		return false;
	else
		return true;

	return false;
}

bool Session::activateAccount(Account *acct) {
	if (!isAuthorized(Session::ACCOUNT_ACTIVATE))
		return false;

	if (typeid(*acct) != typeid(Account))
		return false;

	if (!acct)
		return false;

	acct->unlock();

	if (!m_db->insertAccount(acct))
		return false;
	else
		return true;

	return false;
}

bool Session::printAccountInfo(Account *acct) {
	if (!isAuthorized(Session::ACCOUNT_PRINT_INFO))
		return false;

	if (typeid(*acct) != typeid(Account))
		return false;

	if (!acct)
		return false;

	// TODO: Implement the printing here or create a string and send it to Ui

	return true;
}

bool Session::createCustomer(Customer *customer) {
	if (!isAuthorized(Session::CUSTOMER_CREATE))
		return false;

	if (typeid(*customer) != typeid(Customer))
		return false;

	if (!customer || m_db->retrievePerson(customer->getUserName()))
			return false;

	if (!m_db->insertPerson(customer))
		return false;
	else
		return true;

	return false;
}

bool Session::updateCustomer(Customer *customer) {
	if (!isAuthorized(Session::CUSTOMER_UPDATE))
		return false;

	if (typeid(*customer) != typeid(Customer))
		return false;

	if (!customer)
		return false;
	else {
		Customer *tmp = dynamic_cast<Customer*>(m_db->retrievePerson(customer->getUserName()));
		if (!tmp)
			return false;
	}

	if (!m_db->insertPerson(customer))
		return false;
	else
		return true;

	return false;
}

bool Session::deleteCustomer(Customer *customer) {
	if (!isAuthorized(Session::CUSTOMER_DELETE))
		return false;

	if (typeid(*customer) != typeid(Customer))
		return false;

	if (!customer)
		return false;
	else {
		Customer *tmp = dynamic_cast<Customer*>(m_db->retrievePerson(customer->getUserName()));
		if (!tmp)
			return false;
	}

	if (!m_db->deletePerson(customer))
		return false;
	else
		return true;

	return false;
}

bool Session::printCustomerInfo(Customer *customer) {
	if (!isAuthorized(Session::CUSTOMER_PRINT_INFO))
		return false;

	if (typeid(*customer) != typeid(Customer))
		return false;

	if (!customer)
		return false;

	stringstream ss;
	string status = customer->isLocked() ? "LOCKED" : "UNLOCKED";
	ss << "Customer Id = " << customer->getId() << endl;
	ss << "Customer username = " << customer->getUserName() << endl;
	ss << "Customer First Name = " << customer->getFirstName() << endl;
	ss << "Customer Last Name = " << customer->getLastName() << endl;
	ss << "Customer National ID = " << customer->getNationalId() << endl;
	ss << "Customer Last Name = " << customer->getLastName() << endl;

	this->info = ss.str();

	return true;

	return true;
}

bool Session::ListAllCustomers() {
	if (!isAuthorized(Session::CUSTOMER_LIST_ALL))
		return false;

	vector<Person*> customers = m_db->getAllPersons(Session::CUSTOMER);

	if (customers.size() == 0) {
		return false;
	}

	for (Person *customer : customers) {
		delete customer; // TODO: prints customer information instead of deleting it
	}

	return true;
}

bool Session::printEmployeeInfo() {
	Employee *emp = dynamic_cast<Employee*>(m_user);
	if (!bIsLoggedIn || !emp)
		return false;

	cout << "Employee Id = " << emp->getId() << endl;
	cout << "Employee username = " << emp->getUserName() << endl;
	cout << "Employee First Name = " << emp->getFirstName() << endl;
	cout << "Employee Last Name = " << emp->getLastName() << endl;
	cout << "Employee National ID = " << emp->getNationalId() << endl;
	cout << "Employee Last Name = " << emp->getLastName() << endl;

	return true;
}

bool Session::transfer(Account *from, Account *to, const int sum) {

	if (!from || !to || sum > from->getBalance())
		return false;

	int oldFromBalance = from->getBalance();
	int oldToBalance = to->getBalance();

	int newFromBalance = oldFromBalance - sum;
	int newToBalance = oldToBalance + sum;

	from->setBalance(newFromBalance);
	to->setBalance(newToBalance);

	if (!m_db->insertAccount(from))
		return false;

	if (!m_db->insertAccount(to))
		return false;

	return true;
}

bool Session::deposit(Account *acct, const int sum) {

	if (typeid(*acct) != typeid(Account))
		return false;

	if (!acct)
		return false;

	int newBalance = acct->getBalance() + sum;
	acct->setBalance(newBalance);

	if (!m_db->insertAccount(acct))
		return false;

	return true;
}


/*
 * User interface methods
 * */

void Ui::ui_create_customer() {

	string username;
	string firstname;
	string lastname;
	string nationalid;
	string password;
	string password_confirm;

	cout << "Registering a new customer" << endl;
	cout << endl;

	Customer *tmp = new Customer();
	tmp->setId(m_session->genUserId());

	cout << "User name: ";
	cin >> username;
	cout << "First name: ";
	cin >> firstname;
	cout << "Last name: ";
	cin >> lastname;

	cout << "National ID: ";
	cin >> nationalid;
	cout <<  endl;

	do {
		password = string(getpass("Password: "));
		cout <<  endl;
		password_confirm = string(getpass("Confirm Password: "));
		if (password != password_confirm)
			cerr << "Password mismatch, Please try again" << endl;
	}
	while (password != password_confirm);

	tmp->setUserName(username);
	tmp->setFirstName(firstname);
	tmp->setLastName(lastname);
	tmp->setNationalId(nationalid);
	tmp->setPassword(m_session->encrypt(password));

	tmp->lock();

	tmp->setAccount(nullptr);

	if(!m_session->createCustomer(tmp)) {
		cerr << "Error creating the customer please contact the an  administrator" << endl;
		run();
	}
	else {
		cout << "Customer was created successfully, please login to continue working" << endl;
		delete tmp;
	}

}

void Ui::ui_update_customer() {
	string username;
		string firstname;
		string lastname;
		string nationalid;
		string password;
		string password_confirm;

		Customer *tmp;

		cout << "Updating a Customer" << endl;
		cout << endl;

		do {
			cout << "Customer user name: ";
			cin >> username;
			tmp = m_session->getCustomer(username);
			if (!tmp)
				cerr << "Customer account doesn't exit" << endl;
		} while (!tmp);


		cout << "First name: ";
		cin >> firstname;
		cout << "Last name: ";
		cin >> lastname;
		cout << "National ID: ";
		cin >> nationalid;
		cout <<  endl;

		tmp->setFirstName(firstname);
		tmp->setLastName(lastname);
		tmp->setNationalId(nationalid);

		tmp->isLocked() ? tmp->lock() : tmp->unlock();

		if(!m_session->updateCustomer(tmp)) {
			cerr << "Error updating " << username <<  " please contact the super admin" << endl;
			run();
		}
		else {
			cout << "Customer was updated successfully, please login to continue working" << endl;
		}
}

void Ui::ui_delete_customer() {

	string username = "";
	Customer *tmp;
	cout << "Enter Customer's user name to delete: ";
	cin >> username;
	tmp = m_session->getCustomer(username);
	if (tmp) {
		if (!m_session->deleteCustomer(tmp))
			cerr << "Failed to delete customer acount: " << username << endl;
		else
			cout << "Deleted customer account: " << username << endl;
	}
	else
		cout << "Failed to query the deleting desired Customer: " << username << endl;
}

void Ui::ui_activate_customer() {
	string username;
	Customer *tmp;

	do {
		cout << "Customer username: ";
		cin >> username;
		tmp = m_session->getCustomer(username);
		if (!tmp)
			cerr << "Customer account doesn't exit" << endl;
	} while (!tmp);

	tmp->unlock();

	if (!m_session->updateCustomer(tmp)){
		cerr << "Error activating " << username <<  " please contact the an Administrator" << endl;
		run();
	}
	else
		cout << "Customer was activated successfully, please login to continue working" << endl;
}

void Ui::ui_deactivate_customer() {
	string username;
	Customer *tmp;

	do {
		cout << "Customer username: ";
		cin >> username;
		tmp = m_session->getCustomer(username);
		if (!tmp)
			cerr << "Customer account doesn't exit" << endl;
	} while (!tmp);

	tmp->unlock();

	if (!m_session->updateCustomer(tmp)){
		cerr << "Error activating " << username <<  " please contact the an Administrator" << endl;
		run();
	}
	else
		cout << "Customer was deactivated successfully" << endl;
}

void Ui::ui_print_customer() {

	string username;
	cout << "Customer username: ";
	cin >> username;
	Customer *cust = m_session->getCustomer(username);
	if (!cust) {
		cerr << "No such customer" << endl;
		exit(-1);
	}

	m_session->printCustomerInfo(cust);
	cout << endl;
	cout << m_session->info;
	m_session->info.clear();
	cout << endl;
}

void Ui::ui_listall_customer() {
}

void Ui::ui_create_account() {
	Account *acct = new Account();
	acct->setId(m_session->genAccountId());
	string customer_name = "";
	cout << "Enter customer name: ";
	cin >> customer_name;
	Customer *cust = m_session->getCustomer(customer_name);
	if (!cust) {
		cerr << "No such customer" << endl;
		run();
	}

	acct->setCustomerId(cust->getId());
	acct->setBalance(0);

	if (!m_session->createAccount(acct)){
		cerr << "Error creating an account" << endl;
		run();
	}
	else {
		cout << "Account " << acct->getId() << " created successfully" << endl;
		delete acct;
	}
}

void Ui::ui_update_account() {

	int account_number;
	cout << "Enter account number: ";
	cin >> account_number;

	Account *acct = m_session->getAccount(account_number);

	if (!m_session->createAccount(acct)){
		cerr << "Error creating an account" << endl;
		run();
	}
	else {
		cout << "Account created successfully" << endl;
		delete acct;
	}
}

void Ui::ui_delete_account() {
	int account_number;
	cout << "Enter Account Number: ";
	cin >> account_number;

	Customer *cust = m_session->getCustomerByAccount(account_number);
	if (!cust) {
		Account *tmp = m_session->getAccount(account_number);
		if (!tmp) {
			cout << "No such account" << endl;
		} else {
			if (!m_session->deleteAccount(tmp)) {
				cerr << "Error deleting account" << endl;
				exit(-1);
			} else {
				cout << "Account deleted Successfully" << endl;
			}
		}
	} else {
		cerr << "Can't delete account, Account is associated with a customer"
				<< endl;
		run();
	}
}

void Ui::ui_activate_account() {
	int account_number;
	cout << "Enter Account Number: ";
	cin >> account_number;

	Account *acct = m_session->getAccount(account_number);
	if (!acct){
		cerr << "No such account" <<endl;
		exit(-1);
	}

	acct->unlock();

	if (!m_session->updateAccount(acct)) {
		cerr << "Failed to activate account" << endl;
		run();
	}
	else
		cout << "Account activated succesfully" << endl;
}

void Ui::ui_deactivate_account() {
	int account_number;
	cout << "Enter Account Number: ";
	cin >> account_number;

	Account *acct = m_session->getAccount(account_number);
	if (!acct){
		cerr << "No such account" <<endl;
		exit(-1);
	}

	acct->lock();

	if (!m_session->updateAccount(acct)) {
		cerr << "Failed to deactivate account" << endl;
		run();
	}
	else
		cout << "Account deactivated succesfully" << endl;
}

void Ui::ui_print_account() {
}

void Ui::ui_listall_account() {
}

void Ui::ui_transfer() {
}

void Ui::ui_deposit() {
}

