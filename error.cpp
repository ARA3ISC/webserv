#include "error.hpp"

void    throwError(int nb)
{
    std::string msg = "syntax error (line: " + std::to_string(nb) + ")";
    throw std::runtime_error(msg.c_str());
}
