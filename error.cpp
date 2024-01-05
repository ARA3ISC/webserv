#include "error.hpp"

void        throwError(std::string str, int nb)
{
    std::string msg = str + " (line: " + std::to_string(nb) + ")";
    throw std::runtime_error(msg.c_str());
}

bool invalid_directive(std::string line, int c)
{
    std::vector<std::string> serverAttr;
    serverAttr.push_back("server_name:");
    serverAttr.push_back("listen:");
    serverAttr.push_back("index:");
    serverAttr.push_back("cgi_path:");
    serverAttr.push_back("default_dir_file:");
    serverAttr.push_back("allow_methods:");
    serverAttr.push_back("Error:");
    serverAttr.push_back("upload:");

    std::vector<std::string> locationAttr;
    locationAttr.push_back("dir_listing:");
    locationAttr.push_back("allow_methods:");
    locationAttr.push_back("root:");
    locationAttr.push_back("auto_index:");
    locationAttr.push_back("index:");
    if (!c)
    {
        if (std::find(serverAttr.begin(), serverAttr.end(), getFirstWord(line)) == serverAttr.end())
            return 1;
    }
    else
        if (std::find(locationAttr.begin(), locationAttr.end(), getFirstWord(line)) == locationAttr.end())
            return 1;
    return 0;
}

bool invalidCgi(std::string cgi)
{
    if (cgi != "py" && cgi != "php" && cgi != "purl" )
        return 1;
    return 0;
}

bool invalidMethod(std::vector<std::string> splited)
{
    for (unsigned long i = 1; i < splited.size(); ++i) {
        if (splited[i] != "GET" && splited[i] != "POST" && splited[i] != "DELETE")
            return 1;
    }
    return 0;
}

bool hasDuplicates(const std::vector<std::string>& vec) {

    std::vector<std::string> seenElements;
    for (unsigned long i = 0; i < vec.size(); ++i) {
        if (std::find(seenElements.begin(), seenElements.end(), vec[i]) != seenElements.end()) {
            return true;
        }
        seenElements.push_back(vec[i]);
    }
    return false;
}