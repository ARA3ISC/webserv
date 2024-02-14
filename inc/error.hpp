#ifndef ERROR_HPP
# define ERROR_HPP
 #include "utils.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

void    throwError(std::string str, int nb);
bool    invalid_directive(std::string line, int c);
bool invalidMethod(std::vector<std::string> splited);
bool hasDuplicates(const std::vector<std::string>& vec);

void checkIndentation(std::string s, int c, int &nbline);

#endif
