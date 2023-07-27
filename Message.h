#pragma once

#include <string>
#include <iomanip>

class Message
{
private:
	std::string _dbID;
	std::string _text;
	std::string _sender;
	std::string _recipient;
	std::string _time;

public:
	Message(const std::string& text, const std::string& sender, const std::string& recipient);
	Message(const std::string& dbID, const std::string& text, const std::string& sender, const std::string& recipient);
	Message(const std::string& dbID, const std::string& text, const std::string& sender, const std::string& recipient, const std::string time);

	std::string getDbID();
	std::string getText();
	std::string getSender();
	std::string getRecipient();
	std::string getTime();
	friend std::ostream& operator<< (std::ostream& stream, Message& message);
};

