#include "Chat.h"
#include "BadLogin.h"
#include "BadPassword.h"
#include "Logger.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <string.h>
#if defined(__linux__)
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#define MESSAGE_LENGTH 1024

Chat::Chat() : _activeUser(nullptr),
				_users_count(0),
    _connected(false),
    _database(std::make_shared <DataBase>()),
    _logger(new Logger("log.txt"))
{
}

Chat::~Chat()
{
}

void Chat::createNewUser(const std::string& name, const std::string& login, const std::string& password)
{
	if (isLoginExist(login)) {
		std::cout << "User with login " << login << " is already exists." << std::endl;
		return;
	}		
    std::shared_ptr <User> newUser = std::make_shared <User>(name, login, password);
    addUser(newUser);
    setActiveUser(newUser);
    if (_database->hasConnection()) {
        std::string query = "INSERT into users (name, login, password) VALUES ('" + name + "', '" + login + "', '" + password + "');";
        _database->executeQueryWithoutResult(query);
    }
	
}

void Chat::addUser(const std::shared_ptr<User>& user)
{
	_users.insert(std::make_pair(_users_count++, user));
}

void Chat::updateUnreadedMessages()
{
    if (_database->hasConnection()) {
        std::string query = "SELECT id FROM users WHERE login LIKE '" + getActiveUser()->getLogin() + "';";
        std::vector<std::string> list = _database->queryResult(query);
        query = "UPDATE messages SET status = 'readed' WHERE status LIKE 'received' AND id_receiver = " + list.at(0) + ";";
        _database->executeQueryWithoutResult(query);
    }
}

void Chat::setActiveUser(const std::shared_ptr<User>& user)
{
	if (user != nullptr)
		std::cout << "Hello, " << user->getName() << "! Nice to see you!" << std::endl;
	_activeUser = user;
}

void Chat::login(std::string login, std::string password)
{
	bool found = isLoginExist(login);
	if (!found)
		throw BadLogin();
	for (std::map<int, std::shared_ptr<User>>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->second->getLogin() == login) {
			if (it->second->getPassword() == password) {
				setActiveUser(it->second);
			}
			else
				throw BadPassword();
		}
	}
}

void Chat::writeToOne(std::string text, std::shared_ptr<User> recipient)
{
	std::shared_ptr <Message> shp_mess = std::make_shared<Message>(text, getActiveUser()->getLogin(),
		                                           recipient->getLogin());
        recipient->addMessage(shp_mess);
        if (_database->hasConnection()) {
            std::string query = "SELECT id FROM users WHERE login LIKE '" + getActiveUser()->getLogin() + "';";
            std::vector<std::string> ids = _database->queryResult(query);
            std::string id_sender = ids.at(0);

            query = "SELECT id FROM users WHERE login LIKE '" + recipient->getLogin() + "';";
            ids = _database->queryResult(query);
            std::string id_recipient = ids.at(0);

            query = "INSERT INTO messages (id_sender, id_receiver, text, status) " 
                "VALUES (" + id_sender + ", " + id_recipient + ", '" + text + "', 'sended');";
            _database->executeQueryWithoutResult(query);

            query = "SELECT id FROM messages WHERE id_sender = " + id_sender + " AND id_receiver = " + id_recipient + " AND text LIKE '" + text + "';";
            ids = _database->queryResult(query);
            std::string id_message = ids.at(0);
        }
        
        std::string log = "From " + getActiveUser()->getLogin() + " to " + recipient->getLogin() + "; Time - " + shp_mess->getTime() +
            "; Text - " + text + "\n";
        _logger->writoToFile(log);

#if defined(__linux__)
        if (_connected) {
            char message[MESSAGE_LENGTH];
            // Взаимодействие с сервером
            bzero(message, sizeof(message));
            //std::string textWithUsers = getActiveUser()->getLogin();
            //textWithUsers.append("\n");
            //textWithUsers.append(recipient->getLogin());
            //textWithUsers.append("\n");
            //textWithUsers.append(text);
            strcpy(message, id_message.c_str());
            ssize_t bytes = write(client_socket_file_descriptor, message, sizeof(message));
            // Если передали >= 0  байт, значит пересылка прошла успешно
            if(bytes >= 0){
                std::cout << "Data send to the server successfully.!" << std::endl;
            }
        }
#endif
}

void Chat::writeToAll(const std::string text)
{
	for (std::map<int, std::shared_ptr<User>>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->second->getLogin() != getActiveUser()->getLogin())
                {
                        writeToOne(text, it->second);
		}
	}
}

bool Chat::isLoginExist(const std::string& login)
{
	for (std::map<int, std::shared_ptr<User>>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->second->getLogin() == login) 			
			return true;
	}
	return false;
}

std::shared_ptr<User> Chat::getActiveUser()
{	
	return _activeUser;
}

void Chat::showMenuAddMessege()
{
	std::cout << "Choose recipient mode : 1 - to One , 2 - to All, 3 - Exit \n";
}

std::shared_ptr<User> Chat::getUser(const std::string login)
{
	for (std::map<int, std::shared_ptr<User>>::iterator it = _users.begin(); it != _users.end(); ++it)
		if (it->second->getLogin() == login)				
			return it->second;    
		
		return std::make_shared <User>();
}

std::ostream& operator<< (std::ostream& os, const Chat& ch)
{
	int count(0);
	for (auto& user : ch._users)
	{		
		os << std::setw(3) << " < " << *user.second << " > ";
		count++;
		if (!(count % 6))
			os << std::endl;
	}
	return os;
}

void Chat::showAllLogin()
{
	for (auto& user : _users)
	{
		int count(0);
		std::cout << std::setw(3) << " < " << user.second->getLogin() << " > ";
		count++;
		if (count % 6 == 0)
			std::cout << std::endl;
	}
}


bool Chat::isUnicName(const std::string name)
{
	int count(0);
	
	for (auto& user : _users)
	{
		
		if (user.second->getName() == name)
			count++;
	}
	
	if (count == 1)
		return true;
	
	else 
		return false;

}

std::string Chat::getNameByLogin(const std::string login)
{
	for (auto& user : _users)
	{

		if (user.second->getLogin() == login)
			return user.second->getName();

	}
			std::cout << "Name by " << login << " is not found!\n";
			return "\0";		
	
}

std::string Chat::getLoginByName(const std::string name)
{
	for (auto& user : _users)
	{
		if (user.second->getName() == name)
			return user.second->getLogin();
	}
	std::cout << "Login by " << name << "is not found!\n";
	return "\0";
}


bool Chat::isontheList(const std::string name)
{
	for (auto& tempname : _users)
	{
		if (tempname.second->getName() == name)
			return true;
	}
	std::cout << "bad recipient, try again!\n";
        return false;
}


#if defined(__linux__)
bool Chat::initClientServerMode()
{
    std::cout << "Choose mode (1 - Server, 2 - Client)" << std::endl;
    int mode = 0;
    std::cin >> mode;

    struct sockaddr_in serveraddress, client;
    socklen_t length;
    int port, connection, bind_status, connection_status;
    std::string ip;

    std::cout << "Enter IP" << std::endl;
    std::cin >> ip;

    std::cout << "Enter port" << std::endl;
    std::cin >> port;

    // Установим адрес сервера
    serveraddress.sin_addr.s_addr = inet_addr(ip.c_str());
    // Зададим номер порта
    serveraddress.sin_port = htons(port);
    // Используем IPv4
    serveraddress.sin_family = AF_INET;

    if (mode == 1) {//server
        _connected = initServer(serveraddress);
    } else {//client
        _connected = initClient(serveraddress);
    }
    return _connected;
}

bool Chat::initClient(sockaddr_in serveraddress)
{
    int connection;
    // Создадим сокет
    client_socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket_file_descriptor == -1){
        std::cout << "Creation of Socket failed!" << std::endl;
        return 0;
    }

    // Установим соединение с сервером
    connection = connect(client_socket_file_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress));
    if(connection == -1){
        std::cout << "Connection with the server failed.!" << std::endl;
        return 0;
    }
    return 1;
}

bool Chat::initServer(sockaddr_in serveraddress)
{
    char message[MESSAGE_LENGTH];
    struct sockaddr_in client;
    socklen_t length;
    int connection, bind_status, connection_status;
    // Создадим сокет
    server_socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket_file_descriptor == -1){
        std::cout << "Socket creation failed.!" << std::endl;
        return 0;
    }
    // Привяжем сокет
    bind_status = bind(server_socket_file_descriptor, (struct sockaddr*)&serveraddress,
    sizeof(serveraddress));
    if(bind_status == -1)  {
        std::cout << "Socket binding failed.!" << std::endl;
        return 0;
    }
    // Поставим сервер на прием данных
    connection_status = listen(server_socket_file_descriptor, 5);
    if(connection_status == -1){
    std::cout << "Socket is unable to listen for new connections.!" << std::endl;
   return 0;
    }  else  {
            std::cout << "Server is listening for new connection: " << std::endl;
        }
    length = sizeof(client);
    connection = accept(server_socket_file_descriptor,(struct sockaddr*)&client, &length);
    if(connection == -1)  {
        std::cout << "Server is unable to accept the data from client.!" << std::endl;
        return 0;
    }

    // Communication Establishment
    while(1){
        bzero(message, MESSAGE_LENGTH);
        read(connection, message, sizeof(message));
        //parseMessage(message);

        std::string m = message;
        std::cout << "Data received from client: " << message << std::endl;
        std::string query = "UPDATE messages SET status = 'received' WHERE id = " + m + ";";
        _database->executeQueryWithoutResult(query);

        std::cout << "Server received a new message. Would you like exit and read it?" << std::endl;
        std::string ansver;
        std::cin >> ansver;
        if (ansver == "y" || ansver == "yes")
            break;
    }

    return 1;
}

void Chat::parseMessage(std::string message)
{
    std::string sender = message.substr(0, message.find('\n'));
    message = message.erase(0, message.find('\n')+1);
    std::string recipient = message.substr(0, message.find('\n'));
    std::string text = message.erase(0, message.find('\n')+1);
    if (isontheList(recipient) && isontheList(sender))
        getUser(recipient)->addMessage(std::make_shared<Message>(text, sender, recipient));
    else {
        std::cout << "Data received from client: " <<  message << std::endl;
        std::cout << "But sender or recipient is not exist." << std::endl;
    }
}
#endif

bool Chat::createDBConnection()
{
    unsigned int port = 0;
    std::string ip, namedb, user, password;

    std::cout << "Enter data for database connection:" << std::endl << "IP:" << std::endl;
    std::cin >> ip;

    std::cout << "Port:" << std::endl;
    std::cin >> port;

    std::cout << "Database name:" << std::endl;
    std::cin >> namedb;

    std::cout << "User:" << std::endl;
    std::cin >> user;

    std::cout << "Password:" << std::endl;
    std::cin >> password;

    return _database->connect(ip, port, namedb, user, password);
}

void Chat::getUsersFromDB()
{
    if (!_database->hasConnection()) {
        return;
    }
    std::string name, login, password;
    std::vector<std::string> userslist = _database->queryResult("SELECT id, name, login, password FROM users;");

    int start = 0, messagesStart = 0;
    std::shared_ptr <User> newUser;
    std::shared_ptr <Message> newMessage;

    while (start < userslist.size()) {
        newUser = std::make_shared <User>(userslist.at(start+1), userslist.at(start+2), userslist.at(start+3));
        std::string text = "SELECT messages.id, text, name, date FROM messages join users on id_sender = users.id where id_receiver = " +
            userslist.at(start) + " AND status LIKE 'received'";

        std::vector<std::string> messages = _database->queryResult(text);
        messagesStart = 0;
        while (messagesStart < messages.size()) {
            newMessage = std::make_shared <Message>(messages.at(messagesStart), messages.at(messagesStart + 1), messages.at(messagesStart + 2),
                newUser->getLogin(), messages.at(messagesStart + 3));
            newUser->addMessage(newMessage);
            messagesStart += 4;
        }

        addUser(newUser);
        start += 4;
    }
}
