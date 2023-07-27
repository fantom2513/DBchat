#pragma once
#include <string>
#include <fstream>
#include <shared_mutex>

class Logger
{
private:
	std::fstream _fileStream;
	std::shared_mutex _mutex;

public:
	Logger(std::string filename);
	~Logger();

	bool writoToFile(std::string text);
	std::string readFromFileOneLine();
};

