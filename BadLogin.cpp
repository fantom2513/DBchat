#include "BadLogin.h"

const char* BadLogin::what() const noexcept
{
    return "Entered wrong login!";
}
