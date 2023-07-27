#include "BadPassword.h"

const char* BadPassword::what() const noexcept
{
    return "Wrong password!";
}
