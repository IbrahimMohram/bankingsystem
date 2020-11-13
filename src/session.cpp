/*
 * session.cpp
 *
 *  Created on: Nov 4, 2020
 *      Author: Amira
 */

#include "session.h"

Session::Session() : m_user(nullptr), m_db(nullptr), bIsLoggedIn(false), m_userType(UNKNOWN), m_totalUsers(0), info("") {
	m_db = new Database();
	m_sessionCapabilities.clear();
	m_totalUsers = m_db->getUsersCount();
	if (m_totalUsers == 0)
		m_sessionCapabilities.push_back("First Run");
}

Session::~Session() {
	delete m_db;
	delete m_user;
}

bool Session::isAuthorized(int priv) {
	if (!bIsLoggedIn)
		return false;
	if (m_user->getCaps() & priv)
		return true;
	return false;
}

void Session::setUserType() {
	if (typeid(*m_user) == typeid(Customer))
		m_userType = CUSTOMER;
	else if (typeid(*m_user) == typeid(Employee))
		m_userType = EMPLOYEE;
	else if (typeid(*m_user) == typeid(Admin))
		m_userType = ADMIN;
	else
		m_userType = UNKNOWN;
}

bool Session::login(const string username, const string password) {

	m_sessionCapabilities.clear();
	string unverified_enc_pass = encrypt(password);
	Person *p = m_db->retrievePerson(username);
	if (!p)
		return false;

	if ((p->getPassword() == unverified_enc_pass) && !p->isLocked()) {

		switch (p->getUserType()) {
		case Session::CUSTOMER: {
			Account *acct = m_db->retrieveAccountByCustomer(p->getId());
			if (acct)
				dynamic_cast<Customer*>(p)->setAccount(acct);
			break;
		}
		case Session::EMPLOYEE:
		case Session::ADMIN:
			break;
		default:
			//m_user = nullptr;
			return false;
		}

		setSessionUser(p);
		return true;
	}
	return false;
}

void Session::logout() {
	this->bIsLoggedIn = false;
	delete m_user;
}

bool Session::changePassword(Person *p, const string newpassword) {

	p->setPassword(encrypt(newpassword));
	if (m_db->insertPerson(p))
		return true;

	return false;
}

bool Session::changePassword(const string newpassword) {

	m_user->setPassword(encrypt(newpassword));
	if (m_db->insertPerson(m_user))
		return true;

	return false;
}

void Session::setSessionCapabilities() {

	setUserType();

	switch (m_userType) {

	case CUSTOMER:
		m_sessionCapabilities.push_back("Print my customer Information");
		m_sessionCapabilities.push_back("Transfer money to another Account");
		m_sessionCapabilities.push_back("Deposit");
		m_sessionCapabilities.push_back("Withdraw");
		m_sessionCapabilities.push_back("Print my account Information");
		m_sessionCapabilities.push_back("Change Password");

		break;

	case EMPLOYEE: {
		m_sessionCapabilities.push_back("Change Password");
		m_sessionCapabilities.push_back("Transfer Money between accounts");
		m_sessionCapabilities.push_back("Deposit Money to Customer");
		Employee *tmpemp = dynamic_cast<Employee*>(m_user);
		if (tmpemp->canCreateAccount())
			m_sessionCapabilities.push_back("Create Account");

		if (tmpemp->canUpdateAccount())
			m_sessionCapabilities.push_back("Update Account");

		if (tmpemp->canDeleteAccount())
			m_sessionCapabilities.push_back("Delete Account");

		if (tmpemp->canActivateAccount())
			m_sessionCapabilities.push_back("Activate Account");

		if (tmpemp->canDeactivateAccount())
			m_sessionCapabilities.push_back("Deactivate Account");

		if (tmpemp->canListAllAccounts())
			m_sessionCapabilities.push_back("List All Accounts");

		if (tmpemp->canPrintAccountInfo())
			m_sessionCapabilities.push_back("Print Account Information");

		if (tmpemp->canCreateCustomer())
			m_sessionCapabilities.push_back("Create a new Customer");

		if (tmpemp->canUpdateCustomer())
			m_sessionCapabilities.push_back("Update Existing Customer");

		if (tmpemp->canDeleteCustomer())
			m_sessionCapabilities.push_back("Delete Customer");

		if (tmpemp->canActivateCustomer())
			m_sessionCapabilities.push_back("Activate Customer");

		if (tmpemp->canDeactivateCustomer())
			m_sessionCapabilities.push_back("Deactivate Customer");

		if (tmpemp->canListAllCustomers())
			m_sessionCapabilities.push_back("List All Customers");

		if (tmpemp->canPrintCustomerInfo())
			m_sessionCapabilities.push_back("Print Customer Information");
		break;
	}

	case ADMIN: {
		Admin *tmpadmin = dynamic_cast<Admin*>(m_user);
		if (tmpadmin->canCreateAdmin())
			m_sessionCapabilities.push_back("Create Administrator");

		if (tmpadmin->canUpdateAdmin())
			m_sessionCapabilities.push_back("Update Administrator");

		if (tmpadmin->canDeleteAdmin())
			m_sessionCapabilities.push_back("Delete Administrator");

		if (tmpadmin->canActivateAdmin())
			m_sessionCapabilities.push_back("Activate Administrator");

		if (tmpadmin->canDeactivateAdmin())
			m_sessionCapabilities.push_back("Deactivate Administrator");

		if (tmpadmin->canPrintAdminInfo())
			m_sessionCapabilities.push_back("Print Administrator Information");

		if (tmpadmin->canListAllAdmin())
			m_sessionCapabilities.push_back("List All Administrators");

		if (tmpadmin->canCreateEmployee())
			m_sessionCapabilities.push_back("Create Employee");

		if (tmpadmin->canUpdateEmployee())
			m_sessionCapabilities.push_back("Update Employee");

		if (tmpadmin->canDeleteEmployee())
			m_sessionCapabilities.push_back("Delete Employee");

		if (tmpadmin->canActivateEmployee())
			m_sessionCapabilities.push_back("Activate Employee");

		if (tmpadmin->canDeactivateEmployee())
			m_sessionCapabilities.push_back("Deactivate Employee");

		if (tmpadmin->canPrintEmployeeInfo())
			m_sessionCapabilities.push_back("Print Employee Information");

		if (tmpadmin->canListAllEmployee())
			m_sessionCapabilities.push_back("List All Employees");

		if (tmpadmin->canCreateAccount())
			m_sessionCapabilities.push_back("Create Account");

		if (tmpadmin->canUpdateAccount())
			m_sessionCapabilities.push_back("Update Account");

		if (tmpadmin->canDeleteAccount())
			m_sessionCapabilities.push_back("Delete Account");

		if (tmpadmin->canActivateAccount())
			m_sessionCapabilities.push_back("Activate Account");

		if (tmpadmin->canDeactivateAccount())
			m_sessionCapabilities.push_back("Deactivate Account");

		if (tmpadmin->canListAllAccounts())
			m_sessionCapabilities.push_back("List All Accounts");

		if (tmpadmin->canPrintAccountInfo())
			m_sessionCapabilities.push_back("Print Account Information");

		if (tmpadmin->canCreateCustomer())
			m_sessionCapabilities.push_back("Create a new Customer");

		if (tmpadmin->canUpdateCustomer())
			m_sessionCapabilities.push_back("Update Existing Customer");

		if (tmpadmin->canDeleteCustomer())
			m_sessionCapabilities.push_back("Delete Customer");

		if (tmpadmin->canActivateCustomer())
			m_sessionCapabilities.push_back("Activate Customer");

		if (tmpadmin->canDeactivateCustomer())
			m_sessionCapabilities.push_back("Deactivate Customer");

		if (tmpadmin->canPrintCustomerInfo())
			m_sessionCapabilities.push_back("Print Customer Information");

		if (tmpadmin->canListAllCustomers())
			m_sessionCapabilities.push_back("List All Customers");

		m_sessionCapabilities.push_back("Change Password");

		break;
	}
	default:
		break;
	}
}

string Session::encrypt(const string word) const {
#ifdef __linux__
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, word.c_str(), word.length());
    SHA256_Final(hash, &sha256);
    string result = string();
    stringstream sstream;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    	sstream << hex << (int) hash[i];
    	result = sstream.str();
    }
#elif _WIN32
    // TODO: Windows hashing code should go here, important to modify string called "result"
#endif
    return result;
}

Person* Session::getPerson(const string username) {
	Person *p = m_db->retrievePerson(username);
	if (!p)
		return nullptr;
	return p;
}

Customer* Session::getCustomer(const string username) {
	Customer *cust = dynamic_cast<Customer*>(getPerson(username));
	if (!cust)
		return nullptr;
	return cust;
}

Employee* Session::getEmployee(const string username) {
	Employee *emp = dynamic_cast<Employee*>(getPerson(username));
	if (!emp)
		return nullptr;
	return emp;
}

Account* Session::getAccount(const int accountid) {
	Account *acct = m_db->retrieveAccount(accountid);
	if (!acct)
		return nullptr;
	return acct;
}

Admin* Session::getAdmin(const string username) {
	Admin *admin = dynamic_cast<Admin*>(getPerson(username));
	if (!admin)
		return nullptr;
	return admin;
}

bool Session::firstRun() {
	if (m_totalUsers > 0)
		return false;
	return true;
}

int Session::genUserId() {
	return m_db->generatePersonNumber();
}

int Session::genAccountId() {
	return m_db->generateAccountNumber();
}

Customer* Session::getCustomerByAccount(const int accountid) {
	Customer *cust = m_db->retrieveCustomerByAccount(accountid);
	if (!cust)
		return nullptr;
	return cust;
}

Account* Session::getAccountByCustomer(const int customerid) {
	Account *acct = m_db->retrieveAccountByCustomer(customerid);
	if (!acct)
		return nullptr;
	return acct;
}
