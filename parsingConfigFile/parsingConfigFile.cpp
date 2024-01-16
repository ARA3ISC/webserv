#include "../inc/parsingConfigFile.hpp"
#include "../inc/error.hpp"
#include "../inc/webserv.hpp"
//#include "location.hpp"
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
    // std::cout <<  "|"<<s<<"|" << std::endl;
    int i;
    for (i = 0; i < c; ++i)
    {
        if (!std::isspace(s[i]))
        {

            throwError("Indentation error", nbline);
        }
    }
    if (s[c] && (s[c] == ' ' || s[c] == '\t'))
    {
        std::cout << "[" << s[c] << c << "]" << std::endl;
        throwError("Indentation error", nbline);
    }
}

void fillLocationAttr(std::ifstream &obj, std::string &line, int &nbline, server *s)
{
    location *l = s->createLocation();
    l->setPath(line, nbline);
    while (getline(obj, line) && (rtrim(line).find("server") == std::string::npos))
    {
        nbline++;
        if (line.empty() || is_empty(line.c_str()) || trimStr(line).at(0) == '#')
            continue;

        if (getFirstWord(trimStr(line)) == "dir_listing:")
        {
            l->set_dir_listing(line, nbline);
        }
        else if (getFirstWord(trimStr(line)) == "allow_methods:")
            l->setMethods(line, nbline);
        if (trimStr(line).find("- location") != std::string::npos)
        {
            s->addLocation(*l);
            checkIndentation(line, 4, nbline);
            fillLocationAttr(obj, line, nbline, s);
            delete l;
            return;
        }
        else if (invalid_directive(trimStr(line), 1))
            throwError("Invalid direction error", nbline);
        else
            checkIndentation(line, 8, nbline);
    }
    nbline++;
    s->addLocation(*l);
    // should re-test later after adding this delete
    delete l;
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
        else if (getFirstWord(line) == "index:")
            s->setIndex(line, nbline);
        else if (getFirstWord(line) == "allow_methods:")
            s->setMethods(line, nbline);
        else if (getFirstWord(line) == "cgi_path:")
            s->setCgiPath(line, nbline);
        else if (getFirstWord(line) == "client_max_body_size:")
            s->setMaxBodySize(line, nbline);
        else if (getFirstWord(line) == "error:")
            s->setErrorPages(line, nbline);
        else if (getFirstWord(line) == "upload:")
            s->setUpload(line, nbline);
        else if (trimStr(line).find("- location") != std::string::npos)
        {
            checkIndentation(line, 4, nbline);
            fillLocationAttr(obj, line, nbline, s);
        }
        else if (invalid_directive(trimStr(line), 0))
             throwError("Invalid direction error", nbline);
        if (rtrim(line).find("- server:") != std::string::npos)
        {
            checkIndentation(line, 0, nbline);
            fillServerAttr(obj, nbline);
        }
    }
    webs.addServer(s);
    delete s;
}

void    countServers(std::string filename)
{
    std::string line;
    int count = 0;
    int ln = 0;

    std::ifstream obj;
    obj.open(filename.c_str());
    if (obj.is_open())
    {
        while (getline(obj, line))
        {
            ln++;
            if (line.empty() || is_empty(line.c_str()) || trimStr(line).at(0) == '#' || line.at(0) == 32 || line.at(0) == '\t')
                continue;
            if (rtrim(line.c_str()) != "- server:")
            {
                // std::cout << "-> "<< line << '\n';
                throwError("Syntax error", ln);
            }
            if (rtrim(line.c_str()) == "- server:")
                count++;
        }
        webs.set_serverCount(count);
        // std::cout << webs.get_serverCount() << std::endl;
        obj.close();
    }

}

void missing(std::string directive, int s)
{
    std::stringstream ss;
    ss << s;
    std::string msg;
    msg = "Missing " + directive + " server number " + ss.str();
    throw std::runtime_error(msg.c_str());
}

void    checkMissingData()
{
    int s = 0;
    for (std::vector<server>::iterator it = webs.getServers().begin(); it != webs.getServers().end(); it++)
    {
        s++;
        if (it->getRoot().empty())
            missing("root", s);
    }
}

void    checkServerBlock(std::ifstream &obj)
{
    std::string line;
    int nbline = 0;
    int c = 0;
    while (getline(obj, line))
    {
        nbline++;
        if (line.empty() || is_empty(line.c_str()) || trimStr(line).at(0) == '#' || line.at(0) == 32 || line.at(0) == '\t')
            continue;

        if (rtrim(line) == "- server:")
        {
            c++;
            fillServerAttr(obj, nbline);
            std::reverse(webs.getServers().begin(), webs.getServers().end());
            checkMissingData();
        }
        else
        {
            throwError("Syntax error", nbline);
        }
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

    std::ifstream obj;
    obj.open(filename.c_str());
    if (obj.is_open())
    {
        countServers(filename);
        checkServerBlock(obj);

//        for (std::vector<server>::iterator it = webs.getServers().begin(); it != webs.getServers().end(); it++)
//        {
//            for (std::vector<location>::iterator l = it->getLocations().begin(); l != it->getLocations().end(); ++l)
//            {
//                for (std::vector<std::string>::iterator i = l->getMethods().begin(); i != l->getMethods().end(); i++)
//                {
//                    std::cout << *i << std::endl;
//                }
//            }
//            std::cout << "****\n";
//        }
//        for (std::vector<server>::iterator it = webs.getServers().begin(); it != webs.getServers().end(); it++)
//        {
//            std::cout << it->getRoot() << std::endl;
//            std::cout << "****\n";
//        }

//     std::vector<server>::iterator it = webs.getServers().begin();
//     it++;
//     it++;
//     std::map<int, std::string>::iterator  i = it->get_error_pages().begin();
//     std::cout << i->first << ": " << i->second << std::endl;
//     i++;
//     std::cout << i->first << ": " << i->second << std::endl;

//        if (!webs.getServers()[0].get().empty())
//            std::cout << webs.getServers()[0].getMethods()[0] << std::endl;
//     std::cout << webs.getServers()[2].getRoot() << std::endl;

//     std::cout << webs.getServers()[0].getServer_names()[0] << std::endl;
//     std::cout << webs.getServers()[0].getListen()[0] << std::endl;
//     std::cout << webs.getServers()[0].getRoot() << std::endl;
//     std::cout << webs.getServers()[0].getIndex()[0] << std::endl;
//     std::cout << webs.getServers()[0].getCgiPath().size() << std::endl;
//     std::cout << (webs.getServers()[0].getCgiPath().begin())->first << ": " << (webs.getServers()[0].getCgiPath().begin())->second << std::endl;
//     std::cout << (++webs.getServers()[0].getCgiPath().begin())->first << ": " << (++webs.getServers()[0].getCgiPath().begin())->second << std::endl;
//     std::cout << (++(++webs.getServers()[0].getCgiPath().begin()))->first << ": " << (++(++webs.getServers()[0].getCgiPath().begin()))->second << std::endl;

//     std::cout << webs.getServers()[0].getMaxBodySize() << std::endl;
//     std::cout << webs.getServers()[0].getLocations()[0].get_dir_listing() << std::endl;
//     std::cout << webs.getServers()[0].getLocations()[0].getMethods().size() << std::endl;
//     std::cout << webs.getServers()[0].getLocations()[0].getMethods()[1] << std::endl;
//     std::cout << webs.getServers()[0].getLocations()[0].getMethods()[2] << std::endl;


//        for (std::vector<server>::iterator it = webs.getServers().begin(); it != webs.getServers().end(); it++)
//        {
//            for (std::map<std::string, std::string>::iterator l = it->getCgiPath().begin(); l != it->getCgiPath().end(); ++l)
//            {
//                std::cout << l->first << ": " << l->second << std::endl;
//            }
//            std::cout << "****\n";
//        }

        obj.close();

    }
    else
        throw std::runtime_error("Cannot open file.");
}


webserv getServer(){
    return webs;
}