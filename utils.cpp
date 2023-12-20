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

std::string getSecondWord(std::string line)
{
    int i = 0;
    std::string word;
    while (line[i] && line[i] != ' ' && line[i] != '\t')
        i++;
    while (line[i] && (line[i] == ' ' || line[i] == '\t'))
            i++;
    if (!line[i])
        return NULL;

    while (line[i] && line[i] != ';')
        word += line[i++];

    if (line[i] == ';' && line[i + 1] != '\0')
        throw std::runtime_error("Error: syntax error");
    return word;
}

int countWords(const std::string& str) {
    int count = 0;
    bool inWord = false;

    for (size_t i = 0; i < str.length(); ++i) {
        if (std::isspace(str[i])) {
            if (inWord) {
                count++;
                inWord = false;
            }
        } else {
            inWord = true;
        }
    }
    if (inWord) {
        count++;
    }
    return count;
}

bool is_digit(std::string arg)
{
//    std::cout << arg << std::endl;
    int i = 0;
    while (arg[i])
    {
        if (!std::isdigit(arg[i]))
            return false;
        i++;
    }
    return true;
}