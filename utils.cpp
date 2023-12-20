#include "utils.hpp"

int is_empty(const char *s) {
    while (*s != '\0') {
        if (*s != 32 && *s != '\t')
            return 0;
        s++;
    }
    return 1;
}

std::string getFirstWord(std::string line)
{
    int i = 0;
    std::string word;
    while (line[i] && line[i] != ' ' && line[i] != '\t')
        word += line[i++];

    return word;
}