#ifndef UTILS3_HPP
# define UTILS3_HPP

#include <iostream>
#include "utils.hpp"
#define GREEN "\033[92m"
#define RESET "\033[0m"
#include <cstdlib>
#include <cstring>

void checkSlash(std::string &path);
bool checkMaxBodySize(std::string line);
void printEntryMsg();
long getValue(std::string value);

#endif