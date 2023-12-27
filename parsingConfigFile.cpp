#include "parsingConfigFile.hpp"
#include "error.hpp"
#include "webserv.hpp"
#include <string>


webserv webs;

// void    checkValue(std::string line)
// {
//     if (getFirstWord(line) == "worker_processes")
//         if (!is_digit(getSecondWord(line)))
//             throwError("Invalid value");

// }

std::string rtrim(const std::string &str)
{
    size_t end = str.find_last_not_of(" \t\n\r");

    if (end == std::string::npos)
    {
        return "";
    }
    return str.substr(0, end + 1);
}

void checkIndentation(std::string s, int c, int &nbline)
{
    int i;
    for (i = 0; i < c; ++i)
    {
        if (!std::isspace(s[i]))
        {
            throwError(nbline);
        }
    }
    if (s[c] && (s[c] == ' ' || s[c] == '\t'))
    {
        throwError(nbline);
    }
}

void fillLocationAttr(std::ifstream &obj, std::string &line, int &nbline, server *s)
{
    // ! must get

    location *l = s->createLocation();
    l->setPath(line, nbline);

    while (getline(obj, line) && (rtrim(line).find("server") == std::string::npos))
    {
        nbline++;
        if (line.empty() || is_empty(line.c_str()) || trimStr(line).at(0) == '#')
            continue;

        if (trimStr(line).find("- location") != std::string::npos)
        {
            s->addLocation(*l);
            fillLocationAttr(obj, line, nbline, s);
            return;
        }
        else
            checkIndentation(line, 8, nbline);
        // std::cout <<"[" << line << "]" << std::endl;
    }
    s->addLocation(*l);
}

void fillServerAttr(std::ifstream &obj, int &nbline)
{
    server *s = webs.createServer();

    std::string line;
    while (getline(obj, line))
    {
        nbline++;
        if (line.empty() || is_empty(line.c_str()) || trimStr(line).at(0) == '#')
            continue;

        if (getFirstWord(line) == "server_name:")
            s->set_server_name(line, nbline);
        else if (getFirstWord(line) == "listen:")
            s->set_listen(line, nbline);
        else if (getFirstWord(line) == "root:")
            s->setRoot(line, nbline);
        else if (rtrim(line).find("- location") != std::string::npos)
        {
            checkIndentation(line, 4, nbline);
            fillLocationAttr(obj, line, nbline, s);
        }
        if (rtrim(line).find("- server:") != std::string::npos)
        {
            checkIndentation(line, 0, nbline);
            fillServerAttr(obj, nbline);
            std::cout << "-----------\n";
        }
    }
    // if (webs.get_serverCount() == 1)
    webs.addServer(*s);

}

void    countServers(std::string filename)
{
    std::string line;
    int count = 0;

    std::ifstream obj(filename);
    if (obj.is_open())
    {
        while (getline(obj, line))
        {
            if (rtrim(line.c_str()) == "- server:")
                count++;
        }
        webs.set_serverCount(count);
        // std::cout << webs.get_serverCount() << std::endl;
        obj.close();
    }

}

void checkServerBlock(std::ifstream &obj)
{
    std::string line;
    int nbline = 0;
    int c = 0;
    while (getline(obj, line))
    {
        nbline++;
        if (line.empty() || is_empty(line.c_str()) || trimStr(line).at(0) == '#' || trimStr(line).at(0) == 32 || trimStr(line).at(0) == '\t')
            continue;
        if (rtrim(line) == "- server:")
        {
            c++;
            fillServerAttr(obj, nbline);
            std::reverse(webs.getServers().begin(), webs.getServers().end());

        }
        else
            throwError(nbline);
    }
    if (!c)
        throw std::runtime_error("No server block found");



}

bool checkExtension(const std::string &fileName, const std::string &extension)
{
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos != std::string::npos)
    {
        std::string fileExtension = fileName.substr(dotPos + 1);
        return fileExtension == extension;
    }
    return false;
}

void startParsing(std::string filename)
{
    if (!checkExtension(filename, "yaml") && !checkExtension(filename, "yml"))
        throw std::runtime_error("Invalid extension.");

    std::ifstream obj(filename);
    if (obj.is_open())
    {
        countServers(filename);
        checkServerBlock(obj);
        // for (std::vector<server>::iterator it = webs.getServers().begin(); it != webs.getServers().end(); it++)
        //     {
        //         for (std::vector<location>::iterator i = it->getLocations().begin(); i < it->getLocations().end(); i++)
        //         {
        //             /* code */
        //             std::cout << i->getPath() << std::endl;
        //         }
        //         std::cout << "****\n";
        //     }
        obj.close();
    }
    else
        throw std::runtime_error("Cannot open file.");
}
