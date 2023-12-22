#include "parsingConfigFile.hpp"
#include "error.hpp"
#include "webserv.hpp"

webserv webs;


void    checkValue(std::string line)
{
    if (getFirstWord(line) == "worker_processes")
        if (!is_digit(getSecondWord(line)))
            throwError("Invalid value");

}

std::string rtrim(const std::string& str) {
    size_t end = str.find_last_not_of(" \t\n\r");

    if (end == std::string::npos) {
        return "";
    }
    return str.substr(0, end + 1);
}

void    serverBlock(std::string line)
{

    // while (getline(obj, line))
        std::cout << line << std::endl;
}
void    checkServerBlock(std::ifstream &obj)
{
    std::string line;
    int lineNb = 0;
    // std::vector<std::string> mainDirectives;
    // mainDirectives.push_back("worker_processes");
    // mainDirectives.push_back("error_log");
    // mainDirectives.push_back("pid");
    // mainDirectives.push_back("events");
    // mainDirectives.push_back("http");
    std::streampos offset;
    while (getline(obj, line))
    {
        lineNb++;
        if (line.empty() || is_empty(line.c_str())
            || line.at(0) == '#' || line.at(0) == ' ' || line.at(0) == '\t')
            continue;


        if (rtrim(line) != "- server:")
        {
            std::string error_line = "syntax error (line: " + std::to_string(lineNb) + ")";
            throwError(error_line);
        }
        else
        {
            webs.newServer();
            // std::cout << webs.get_serverCount() << std::endl;
            while (getline(obj, line) && line[0] != '-')
            {
                if (line[0] == '-')
                {
                    offset = obj.tellg();
                    obj.seekg(offset);
                    break;
                }
                std::cout << " = " << line << std::endl;
            }
        }
    }
    std::cout << "server count: " << webs.get_serverCount() << std::endl;
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

