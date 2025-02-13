/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maneddam <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 16:05:24 by maneddam          #+#    #+#             */
/*   Updated: 2024/02/14 16:05:25 by maneddam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
    size_t start = str.find_first_not_of(" \t\n\r");

    if (start == std::string::npos) {
        return "";
    }

    size_t end = str.find_last_not_of(" \t\n\r");

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

std::string removeLastColon(std::string& input) {
    if (input.empty()) {
        throw std::invalid_argument("Empty string provided.");
    }

    std::string result = input.substr(0, input.length() - 1);
    return result;
}

std::vector<std::string> splitBy(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        if(token.empty())
            continue;
        tokens.push_back(token);
    }

    return tokens;
}
