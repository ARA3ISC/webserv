#ifndef ERROR_HPP
# define ERROR_HPP
 #include "utils.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

void    throwError(std::string str, int nb);
bool invalidCgi(std::string cgi);
bool    invalid_directive(std::string line, int c);
bool invalidMethod(std::vector<std::string> splited);
bool hasDuplicates(const std::vector<std::string>& vec);

template<class T>
void checkIndentation(std::string s, int c, int &nbline, T* allocated, bool deleteit)
{
    // std::cout <<  "|"<<s<<"|" << std::endl;
    int i;
    for (i = 0; i < c; ++i)
    {
        if (!std::isspace(s[i]))
        {
            if (deleteit)
                delete allocated;
            throwError("Indentation error", nbline);
        }
    }
    if (s[c] && (s[c] == ' ' || s[c] == '\t'))
    {
        if (deleteit)
            delete allocated;
        throwError("Indentation error", nbline);
    }
}

#endif
