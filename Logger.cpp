#include "Logger.h"
#include <iostream>

Logger::Logger(std::string filename) {
	_fileStream = std::fstream(filename, std::ios::app);
	if (!_fileStream.is_open()) {
		std::cout << "Could not open file " << filename << " !" << '\n';
	}
}
Logger::~Logger() {
	_fileStream.close();
}

bool Logger::writoToFile(std::string text)
{
	_mutex.lock();
	_fileStream << text;
	_mutex.unlock();
	return false;
}

std::string Logger::readFromFileOneLine()
{
	std::string result;
	_mutex.lock_shared();
	std::getline(_fileStream, result);
	_mutex.unlock_shared();
	return result;
}
