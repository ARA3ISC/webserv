#ifndef UTILS_HPP
# define UTILS_HPP

#include <iostream>
#include <sstream>
#include <vector>


int is_empty(const char *s);
std::string getFirstWord(std::string line);
std::string getSecondWord(std::string line);
int countWords(const std::string& str);
bool is_digit(std::string arg);

std::vector<std::string> splitBySpace(const std::string& input);
std::string trimStr(const std::string& str);
#endif
