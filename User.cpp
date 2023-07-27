#include "User.h"
#include <iostream>
#include <fstream>
#include <string.h>

User::User() = default;

User::User(const std::string& name, const std::string& login, const std::string& password) :
	_name(name), _login(login), _password(password) {
}

void User::addMessage(std::shared_ptr<Message> msg)
{
    _messages.push(msg);
}

void User::showUnreadedMessages()
{
	if (_messages.size() > 0) {
		std::cout << _name << ", your have unreaded messages: " << std::endl << std::endl;
		while (_messages.size() > 0) {
			std::shared_ptr<Message> msg = _messages.front();
			std::cout << msg->getSender() << std::setw(35) << msg->getTime() << std::setw(30)
				<< "--->" << std::setw(30)
				<< msg->getRecipient() << std::endl << std::endl << std::setw(20) << "\" "
				<< msg->getText() << " \""
				<< std::endl << std::endl;		
			_messages.pop();
		}
	}
	else {
		std::cout << "You have not unreaded messages." << std::endl;
	}
}

std::ostream& operator<< (std::ostream& os, const User& us)
{
	os << us._name;
	return os;
}
