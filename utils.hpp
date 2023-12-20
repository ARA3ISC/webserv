#pragma  once

#include "webserv.hpp"
int is_empty(const char *s);
std::string getFirstWord(std::string line);
std::string getSecondWord(std::string line);
int countWords(const std::string& str);
bool is_digit(std::string arg);