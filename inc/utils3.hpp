#pragma once

#include <iostream>
#include "utils.hpp"
#define GREEN "\033[92m"
#define RESET "\033[0m"
#include <cstdlib>
#include <cstring>


//std::string validPath(std::string path);
void checkSlash(std::string &path);
bool checkMaxBodySize(std::string line);
void printEntryMsg();
long getValue(std::string value);
