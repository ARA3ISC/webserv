#include "../inc/utils2.hpp"
#include "../inc/utils.hpp"

std::vector<std::string> splitHeaderBycolon(std::string line) {

    std::string key;
    std::string value;

    std::vector<std::string> splited;
    int i = 0;
    while (line[i] && line[i] != ':')
        key += line[i++];

    if (line[i] == ':')
        i++;
    while (line[i])
        value += line[i++];

    splited.push_back(trimStr(key));
    splited.push_back(trimStr(value));
    return splited;
}