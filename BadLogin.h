#pragma once
#include <exception>

class BadLogin : public std::exception
{
public:
	virtual const char* what() const noexcept override;
};
