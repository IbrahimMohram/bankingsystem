/*
 * admin.cpp

 *
 *  Created on: Oct 19, 2020
 *      Author: amira
 */
#include <assert.h>
#include <typeinfo>
#include "session.h"
#include "userinterface.h"
#ifdef __linux__
#include <unistd.h>
#endif

bool Session::createAdmin(Admin *admin) {
	if (!isAuthorized(Session::ADMIN_CREATE) && m_totalUsers > 0)
		return false;

	assert(typeid(*admin) == typeid(Admin));

	if (!admin || m_db->retrievePerson(admin->getUserName()))
			return false;

	if (m_db->insertPerson(admin))
		return true;

	return false;
}

bool Session::updateAdmin(Admin *admin) {
	if (!isAuthorized(Session::ADMIN_UPDATE))
		return false;

	assert(typeid(*admin) == typeid(Admin));

	if (!admin)
		return false;
	else {
		Admin *tmp = dynamic_cast<Admin*>(m_db->retrievePerson(admin->getUserName()));
		if (!tmp)
			return false;
	}

	if (m_db->insertPerson(admin))
		return true;

	return false;

}

bool Session::deleteAdmin(Admin *admin) {
	if (!isAuthorized(Session::ADMIN_DELETE))
		return false;

	assert(typeid(*admin) == typeid(Admin));

	if (!admin)
		return false;
	else {
		Admin *tmp = dynamic_cast<Admin*>(m_db->retrievePerson(admin->getUserName()));
		if (!tmp)
			return false;
	}

	if (m_db->deletePerson(admin))
		return true;

	return false;

}

bool Session::deactivateAdmin(Admin *admin) {
	if (!isAuthorized(Session::ADMIN_DEACTIVATE))
		return false;

	assert(typeid(*admin) == typeid(Admin));

	if (!admin || !m_db->retrievePerson(admin->getUserName()))
		return false;

	admin->lock();

	if (m_db->insertPerson(admin))
		return true;

	return false;
}

bool Session::activateAdmin(Admin *admin) {
	if (!isAuthorized(Session::ADMIN_ACTIVATE))
		return false;

	assert(typeid(*admin) == typeid(Admin));

	if (!admin || !m_db->retrievePerson(admin->getUserName()))
		return false;

	admin->unlock();

	if (m_db->insertPerson(admin))
		return true;

	return false;
}

bool Session::printAdminInfo(Admin *admin) {
	if (!isAuthorized(Session::ADMIN_PRINT_INFO))
		return false;

	assert(typeid(*admin) == typeid(Admin));

	if (!admin || !m_db->retrievePerson(admin->getUserName()))
		return false;

	// TODO: Implement admin print info here

	return true;
}

bool Session::printAdminInfo() {
	Admin *admin = dynamic_cast<Admin*>(m_user);
	if (!bIsLoggedIn || !admin)
		return false;

	cout << "Admin Id = " << admin->getId() << endl;
	cout << "Admin username = " << admin->getUserName() << endl;
	cout << "Admin First Name = " << admin->getFirstName() << endl;
	cout << "Admin Last Name = " << admin->getLastName() << endl;
	cout << "Admin National ID = " << admin->getNationalId() << endl;
	cout << "Admin Last Name = " << admin->getLastName() << endl;

	return true;
}

bool Session::ListAllAdmins() {
	if (!isAuthorized(Session::ADMIN_LIST_ALL))
		return false;

	vector<Person*> admins = m_db->getAllPersons(Session::ADMIN);

	if (admins.size() == 0) {
		return false;
	}

	for (Person *admin : admins) {
		delete admin; // TODO: prints admin information instead of deleting it
	}

	return true;
}

bool Session::createEmployee(Employee *emp) {
	if (!isAuthorized(Session::EMPLOYEE_CREATE))
		return false;

	assert(typeid(*emp) == typeid(Employee));

	if (!emp || m_db->retrievePerson(emp->getUserName()))
		return false;

	if (m_db->insertPerson(emp))
		return true;

	return false;
}

bool Session::updateEmployee(Employee *emp) {
	if (!isAuthorized(Session::EMPLOYEE_UPDATE))
		return false;

	assert(typeid(*emp) == typeid(Employee));

	if (!emp)
		return false;
	else {
		Employee *tmp = dynamic_cast<Employee*>(m_db->retrievePerson(emp->getUserName()));
		if (!tmp)
			return false;
	}

	if (m_db->insertPerson(emp))
		return true;

	return false;
}

bool Session::deleteEmployee(Employee *emp) {
	if (!isAuthorized(Session::EMPLOYEE_DELETE))
		return false;

	assert(typeid(*emp) == typeid(Employee));

	if (!emp)
		return false;
	else {
		Employee *tmp = dynamic_cast<Employee*>(m_db->retrievePerson(emp->getUserName()));
		if (!tmp)
			return false;
	}

	if (m_db->deletePerson(emp))
		return true;

	return false;
}

bool Session::activateEmployee(Employee *emp) {
	if (!isAuthorized(Session::EMPLOYEE_ACTIVATE))
		return false;

	assert(typeid(*emp) == typeid(Employee));

	if (!emp || !m_db->retrievePerson(emp->getUserName()))
		return false;

	emp->unlock();

	if (m_db->insertPerson(emp))
		return true;

	return false;
}

bool Session::deactivateEmployee(Employee *emp) {
	if (!isAuthorized(Session::EMPLOYEE_DEACTIVATE))
		return false;

	assert(typeid(*emp) == typeid(Employee));

	if (!emp || !m_db->retrievePerson(emp->getUserName()))
		return false;

	emp->lock();

	if (m_db->insertPerson(emp))
		return true;

	return false;
}

bool Session::printEmployeeInfo(Employee *emp) {
	if (!isAuthorized(Session::EMPLOYEE_PRINT_INFO))
		return false;

	assert(typeid(*emp) == typeid(Employee));

	if (!emp || !m_db->retrievePerson(emp->getUserName()))
		return false;

	return true;
}

bool Session::ListAllEmployees() {
	if (!isAuthorized(Session::EMPLOYEE_LIST_ALL))
		return false;

	vector<Person*> employees = m_db->getAllPersons(Session::EMPLOYEE);

	if (employees.size() == 0) {
		return false;
	}

	for (Person *employee : employees) {
		delete employee; // TODO: prints employee information instead of deleting it
	}

	return true;
}


void Ui::ui_create_admin() {

	string username;
	string firstname;
	string lastname;
	string nationalid;
	string password;
	string password_confirm;

	cout << "Registering an administrator" << endl;
	cout << endl;

	Admin *tmp = new Admin();
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

	string answer = "";

	do {
		cout << "Can create other Administrators? (y/N): ";
		cin >> answer;
	} while(!cin.fail() && (answer!="y" && answer!="n" && answer!="Y" &&answer != "N"));

	if (answer == "y" || answer == "Y")
		tmp->cap_AdminCreate(true);
	else
		tmp->cap_AdminCreate(false);
	answer = "";


	do {
		cout << "Can Update other Administrators? (y/N): ";
		cin >> answer;
	} while(!cin.fail() && (answer!="y" && answer!="n" && answer!="Y" &&answer != "N"));

	if (answer == "y" || answer == "Y")
		tmp->cap_AdminUpdate(true);
	else
		tmp->cap_AdminUpdate(false);
	answer = "";

	do {
		cout << "Can Delete other Administrators? (y/N): ";
		cin >> answer;
	} while(!cin.fail() && (answer!="y" && answer!="n" && answer!="Y" &&answer != "N"));

	if (answer == "y" || answer == "Y")
		tmp->cap_AdminDelete(true);
	else
		tmp->cap_AdminDelete(false);
	answer = "";

	do {
		cout << "Can Activate other Administrators? (y/N): ";
		cin >> answer;
	} while(!cin.fail() && (answer!="y" && answer!="n" && answer!="Y" &&answer != "N"));

	if (answer == "y" || answer == "Y")
		tmp->cap_AdminActivate(true);
	else
		tmp->cap_AdminActivate(false);
	answer = "";


	do {
		cout << "Can Deactivate other Administrators? (y/N): ";
		cin >> answer;
	} while(!cin.fail() && (answer!="y" && answer!="n" && answer!="Y" &&answer != "N"));

	if (answer == "y" || answer == "Y")
		tmp->cap_AdminDeactivate(true);
	else
		tmp->cap_AdminDeactivate(false);
	answer = "";


	do {
		cout << "Can list all other Administrators? (y/N): ";
		cin >> answer;
	} while(!cin.fail() && (answer!="y" && answer!="n" && answer!="Y" &&answer != "N"));

	if (answer == "y" || answer == "Y")
		tmp->cap_AdminListAll(true);
	else
		tmp->cap_AdminListAll(false);
	answer = "";


	do {
		cout << "Can print Administrators info? (y/N): ";
		cin >> answer;
	} while(!cin.fail() && (answer!="y" && answer!="n" && answer!="Y" &&answer != "N"));

	if (answer == "y" || answer == "Y")
		tmp->cap_AdminPrintInfo(true);
	else
		tmp->cap_AdminPrintInfo(false);
	answer = "";


	if(!m_session->createAdmin(tmp)) {
		cerr << "Error creating the administrator please contact the super admin" << endl;
		exit(-1);
	}
	else {
		cout << "Administrator was created successfully, please login to continue working" << endl;
		delete tmp;
	}
}

void Ui::ui_update_admin() {

	string username;
	string firstname;
	string lastname;
	string nationalid;
	string password;
	string password_confirm;
	string answer = "";
	Admin *tmp;

	cout << "Updating an administrator" << endl;
	cout << endl;

	do {
		cout << "Admin user name: ";
		cin >> username;
		tmp = m_session->getAdmin(username);
		if (!tmp)
			cerr << "Admin account doesn't exit" << endl;
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

	do {
		cout << "Can create other Administrators? (y/N): ";
		cin >> answer;
	} while(!cin.fail() && (answer!="y" && answer!="n" && answer!="Y" &&answer != "N"));

	if (answer == "y" || answer == "Y")
		tmp->cap_AdminCreate(true);
	else
		tmp->cap_AdminCreate(false);
	answer = "";


	do {
		cout << "Can Update other Administrators? (y/N): ";
		cin >> answer;
	} while(!cin.fail() && (answer!="y" && answer!="n" && answer!="Y" &&answer != "N"));

	if (answer == "y" || answer == "Y")
		tmp->cap_AdminUpdate(true);
	else
		tmp->cap_AdminUpdate(false);
	answer = "";

	do {
		cout << "Can Delete other Administrators? (y/N): ";
		cin >> answer;
	} while(!cin.fail() && (answer!="y" && answer!="n" && answer!="Y" &&answer != "N"));

	if (answer == "y" || answer == "Y")
		tmp->cap_AdminDelete(true);
	else
		tmp->cap_AdminDelete(false);
	answer = "";

	do {
		cout << "Can Activate other Administrators? (y/N): ";
		cin >> answer;
	} while(!cin.fail() && (answer!="y" && answer!="n" && answer!="Y" &&answer != "N"));

	if (answer == "y" || answer == "Y")
		tmp->cap_AdminActivate(true);
	else
		tmp->cap_AdminActivate(false);
	answer = "";


	do {
		cout << "Can Deactivate other Administrators? (y/N): ";
		cin >> answer;
	} while(!cin.fail() && (answer!="y" && answer!="n" && answer!="Y" &&answer != "N"));

	if (answer == "y" || answer == "Y")
		tmp->cap_AdminDeactivate(true);
	else
		tmp->cap_AdminDeactivate(false);
	answer = "";


	do {
		cout << "Can list all other Administrators? (y/N): ";
		cin >> answer;
	} while(!cin.fail() && (answer!="y" && answer!="n" && answer!="Y" &&answer != "N"));

	if (answer == "y" || answer == "Y")
		tmp->cap_AdminListAll(true);
	else
		tmp->cap_AdminListAll(false);
	answer = "";


	do {
		cout << "Can print Administrators info? (y/N): ";
		cin >> answer;
	} while(!cin.fail() && (answer!="y" && answer!="n" && answer!="Y" &&answer != "N"));

	if (answer == "y" || answer == "Y")
		tmp->cap_AdminPrintInfo(true);
	else
		tmp->cap_AdminPrintInfo(false);
	answer = "";


	if(!m_session->updateAdmin(tmp)) {
		cerr << "Error updating " << username <<  " please contact the super admin" << endl;
		exit(-1);
	}
	else {
		cout << "Administrator was updated successfully, please login to continue working" << endl;
	}
}

void Ui::ui_delete_admin() {

	string username = "";
	Admin *tmp;
	cout << "Enter administrator's user name to delete: ";
	cin >> username;
	tmp = m_session->getAdmin(username);
	if (tmp) {
		if (!m_session->deleteAdmin(tmp))
			cerr << "Failed to delete admin acount: " << username << endl;
		else
			cout << "Deleted admin account: " << username << endl;
	}
	else
		cout << "Failed to query the deleting desired Admin: " << username << endl;
}

void Ui::ui_activate_admin() {
	string username;
	Admin *tmp;

	do {
		cout << "Admin username: ";
		cin >> username;
		tmp = m_session->getAdmin(username);
		if (!tmp)
			cerr << "Admin account doesn't exit" << endl;
	} while (!tmp);

	tmp->unlock();

	if (!m_session->updateAdmin(tmp)){
		cerr << "Error activating " << username <<  " please contact the super admin" << endl;
		exit(-1);
	}
	else
		cout << "Administrator was activated successfully, please login to continue working" << endl;
}

void Ui::ui_deactivate_admin() {
	string username;
	Admin *tmp;

	do {
		cout << "Admin username: ";
		cin >> username;
		tmp = m_session->getAdmin(username);
		if (!tmp)
			cerr << "Admin account doesn't exit" << endl;
	} while (!tmp);

	tmp->lock();

	if (!m_session->updateAdmin(tmp)){
		cerr << "Error deactivating " << username <<  " administrator please contact the super admin" << endl;
		exit(-1);
	}
	else
		cout << "Administrator was deactivated successfully" << endl;
}

void Ui::ui_print_admin() {
}

void Ui::ui_listall_admin() {
}

void Ui::ui_create_employee() {
}

void Ui::ui_update_employee() {
}

void Ui::ui_delete_employee() {

	string username = "";
	Employee *tmp;
	cout << "Enter Emplyee's user name to delete: ";
	cin >> username;
	tmp = m_session->getEmployee(username);
	if (tmp) {
		if (!m_session->deleteEmployee(tmp))
			cerr << "Failed to delete employee acount: " << username << endl;
		else
			cout << "Deleted employee account: " << username << endl;
	}
	else
		cout << "Failed to query the deleting desired Employee: " << username << endl;

}

void Ui::ui_activate_employee() {
}

void Ui::ui_deactivate_employee() {
}

void Ui::ui_print_employee() {
}

void Ui::ui_listall_employee() {
}
