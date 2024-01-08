#include "../inc/utils.hpp"

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

    while (line[i] == 32 || line[i] == '\t')
        i++;
    while (line[i] && line[i] != ' ' && line[i] != '\t')
        word += line[i++];

    return word;
}

// std::string     getSecondWord(std::string line)
// {
//     int i = 0;
//     std::string word;
//     while (line[i] && line[i] != ' ' && line[i] != '\t')
//         i++;
//     while (line[i] && (line[i] == ' ' || line[i] == '\t'))
//             i++;
//     if (!line[i])
//         return NULL;

//     while (line[i])
//         word += line[i++];

//     if (line[i] == ';' && line[i + 1] != '\0')
//         throw std::runtime_error("Error: syntax error");
//     return word;
// }

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
    int i = 0;
    while (arg[i])
    {
        if (!std::isdigit(arg[i]))
            return false;
        i++;
    }
    return true;
}

std::vector<std::string> splitBySpace(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> words;
    std::string word;

    while (iss >> word) {
        words.push_back(word);
    }

    return words;
}

std::string trimStr(const std::string& str) {
    // Find the first non-whitespace character from the beginning
    size_t start = str.find_first_not_of(" \t\n\r");

    // If the string is empty or contains only whitespaces
    if (start == std::string::npos) {
        return "";
    }

    // Find the last non-whitespace character from the end
    size_t end = str.find_last_not_of(" \t\n\r");

    // Return the trimmed substring
    return str.substr(start, end - start + 1);
}

bool isNaN(std::string str)
{
    int i = 0;
    while (str[i])
    {
        if (!isdigit(str[i]))
            return true;
        i++;
    }
    return false;
}

void    removeComment(std::vector<std::string>& splited)
{
    for (unsigned long i = 0; i < splited.size(); ++i) {
        if (splited[i].at(0) == '#') {
            splited.erase(splited.begin() + i, splited.end());
            break;
        }
    }
}

std::string removeLastColon(const std::string& input) {
    if (input.empty()) {
        throw std::invalid_argument("Empty string provided.");
    }

    if (input.back() != ':') {
        throw std::invalid_argument("No colon at the end of the string.");
    }
    std::string result = input.substr(0, input.length() - 1);
    return result;
}