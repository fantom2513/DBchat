#pragma once
#include <exception>

class BadPassword : public std::exception
{
public:
	virtual const char* what() const noexcept override;
};
