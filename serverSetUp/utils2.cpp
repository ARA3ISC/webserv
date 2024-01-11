#include "../inc/utils2.hpp"
#include "../inc/utils.hpp"

std::vector<std::string> splitHeaderBycolon(std::string line) {

    std::string key;
    std::string value;

    std::vector<std::string> splited;
    int i = 0;
    while (line[i] && line[i] != ':')
        key += line[i++];

    if (line[i] != ':')
        throw std::runtime_error("Bad request 400");
    else
    {
        if (line[i - 1] == ' ' || line[i - 1] == '\t')
            throw std::runtime_error("Bad request 400");
        i++;
    }
    while (line[i])
        value += line[i++];

    splited.push_back(key);
    splited.push_back(trimStr(value));
    return splited;
}

std::string trimFromBeginning(const std::string& input, const std::string& prefix) {
    size_t pos = input.find(prefix);

    if (pos != std::string::npos) {
        // Found the prefix, trim from the beginning
        return input.substr(pos + prefix.length());
    } else {
        // Prefix not found, return the original string
        return input;
    }
}