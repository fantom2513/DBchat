#pragma once
#include <ostream>
#include <string>
#include <queue>
#include <memory>
#include <fstream>
#include "Message.h"

class User
{
	std::string _name;
	std::string _login;
	std::string _password;
	std::queue <std::shared_ptr <Message>> _messages;

public:
	User();
	User(const std::string& name, const std::string& login, const std::string& password);	
	std::string& getName() { return _name; }
	const std::string& getLogin() { return _login; }
	std::string& getPassword() { return _password; }
	friend std::ostream& operator << (std::ostream&, const User&);
    void addMessage(std::shared_ptr <Message> msg);
	void showUnreadedMessages();
};

