#include "parsingConfigFile.hpp"
#include "error.hpp"
#include "webserv.hpp"

webserv webs;

// void    checkValue(std::string line)
// {
//     if (getFirstWord(line) == "worker_processes")
//         if (!is_digit(getSecondWord(line)))
//             throwError("Invalid value");

// }

std::string rtrim(const std::string& str) {
    size_t end = str.find_last_not_of(" \t\n\r");

    if (end == std::string::npos) {
        return "";
    }
    return str.substr(0, end + 1);
}

void    checkIndentation(std::string s, int c, int nbline)
{
    int i;
    for (i = 0; i < c; ++i) {
        if (!std::isspace(s[i])) {
            // std::cout << "[" << line[i] << "]\n";
            throwError(nbline);
        }
    }
    if (s[i] == 32 || s[i] == '\t')
        throwError(nbline);
}

void    fillLocationAttr(std::ifstream &obj, std::string line, int nbline)
{
    // ! must get

    while (getline(obj, line) && rtrim(line) != "- server:")
    {
        nbline++;
        if (line.empty() || is_empty(line.c_str())
            || line.at(0) == '#')
            continue;


        if (rtrim(line).find("- location") != std::string::npos)
        {
            checkIndentation(line, 4, nbline);
            fillLocationAttr(obj, line, nbline);
            // std::cout << "-------\n";
        }
        else
        {
        // std::cout << "-"  << line << "-" << std::endl;
            checkIndentation(line, 8, nbline);
        }
    }

}

void    fillServerAttr(std::ifstream &obj, int nbline)
{
    std::string line;
    while (getline(obj, line) && rtrim(line) != "- server:")
    {
        nbline++;
        if (line.empty() || is_empty(line.c_str())
            || line.at(0) == '#')
            continue;

        checkIndentation(line, 4, nbline);

        if (rtrim(line).find("- location") != std::string::npos)
        {
            // std::cout << line << '\n';
            fillLocationAttr(obj, line, nbline);


        }
    }

}

void    checkServerBlock(std::ifstream &obj)
{
    std::string line;
    int lineNb = 0;
    while (getline(obj, line))
    {
        lineNb++;
        if (line.empty() || is_empty(line.c_str())
            || line.at(0) == '#' || line.at(0) == ' ' || line.at(0) == '\t')
            continue;
        if (rtrim(line) != "- server:")
            throwError(lineNb);
        else
        {
            fillServerAttr(obj, lineNb);

        }
    }
//    std::cout << "server count: " << webs.get_serverCount() << std::endl;
}

bool checkExtension(const std::string& fileName, const std::string& extension) {
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string fileExtension = fileName.substr(dotPos + 1);
        return fileExtension == extension;
    }
    return false;
}

void    startParsing(std::string filename)
{
    if (!checkExtension(filename, "yaml") && !checkExtension(filename, "yml") )
        throw std::runtime_error("Invalid extension.");

    std::ifstream obj(filename);
    if (obj.is_open())
    {
        checkServerBlock(obj);
        obj.close();
    }
    else
        throw std::runtime_error("Cannot open file.");
}

