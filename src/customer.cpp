/*
 * customer.cpp
 *
 *  Created on: Nov 6, 2020
 *      Author: Amira
 */

#include <sstream>
#include "session.h"
#include "userinterface.h"

using namespace std;

/*
 * Session methods
 * */

bool Session::withdraw(const int sum) {

	Customer *cust = dynamic_cast<Customer*>(m_user);
	if (!cust || !bIsLoggedIn)
		return false;

	int oldBalance = cust->getAccount()->getBalance();
	if (cust->getAccount()->getBalance() < sum) {
		return false;
	}

	cust->getAccount()->setBalance(oldBalance - sum);

	// Stricktly verify since it is user money
	if (cust->getAccount()->getBalance() == (oldBalance - sum))
		if (m_db->insertAccount(cust->getAccount()))
			return true;

	return false;
}

bool Session::transfer(const int to, const int sum) {

	Customer *cust = dynamic_cast<Customer*>(this->m_user);
	if (!cust || !bIsLoggedIn)
		return false;

	int oldBalance = cust->getAccount()->getBalance();
	if (cust->getAccount()->getBalance() < sum)
		return false;


	Account *toAccount = m_db->retrieveAccount(to);
	if (toAccount) {
		cust->getAccount()->setBalance(oldBalance - sum);
		int oldAccountBalance = toAccount->getBalance();
		toAccount->setBalance(oldAccountBalance + sum);

		// Stricktly verify since it is user money
		if (toAccount->getBalance() == (oldAccountBalance + sum))
			if (m_db->insertAccount(cust->getAccount())
					&& m_db->insertAccount(toAccount))
				return true;
	}
	return false;
}

bool Session::deposit(const int sum) {

	Customer *cust = dynamic_cast<Customer*>(m_user);
	if (!cust || !bIsLoggedIn)
		return false;

	int oldBalance = cust->getAccount()->getBalance();
	cust->getAccount()->setBalance(sum + oldBalance);

	// Stricktly verify since it is user money
	if (cust->getAccount()->getBalance() == (oldBalance + sum))
		if (m_db->insertAccount(cust->getAccount()))
			return true;

	return false;
}

bool Session::printCustomerInfo() {

	Customer *cust = dynamic_cast<Customer*>(m_user);
	if (!bIsLoggedIn || !cust)
		return false;

	stringstream ss;
	string status = cust->isLocked() ? "LOCKED" : "UNLOCKED";
	ss << "Customer Id = " << cust->getId() << endl;
	ss << "Customer username = " << cust->getUserName() << endl;
	ss << "Customer First Name = " << cust->getFirstName() << endl;
	ss << "Customer Last Name = " << cust->getLastName() << endl;
	ss << "Customer National ID = " << cust->getNationalId() << endl;
	ss << "Customer account status = " << status << endl;

	this->info = ss.str();

	return true;
}

bool Session::printAccountInfo() {

	Customer *cust = dynamic_cast<Customer*>(m_user);
	if (!bIsLoggedIn || !cust->getAccount())
		return false;

	Account *acct = cust->getAccount();
	string status = acct->isLocked() ? "LOCKED" : "UNLOCKED";
	cout << "Account Number: " << acct->getId()
			<< ",  Account Balance: " << acct->getBalance()
			<< ", Status: " << status <<endl;

	return true;
}

/*
 * User interface methods
 * */

void Ui::ui_transfer_own() {
}

void Ui::ui_deposit_own() {
}

void Ui::ui_withdraw() {
}

void Ui::ui_print_own_customer() {

	cout << endl;
	m_session->printCustomerInfo();
	cout << m_session->info;
	m_session->info.clear();
	cout << endl;

}
