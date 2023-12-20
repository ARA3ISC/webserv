#include "error.hpp"

void    throwError(std::string msg)
{
    throw std::runtime_error(msg.c_str());
}