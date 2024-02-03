#include "../inc/parsingConfigFile.hpp"
#include "../inc/error.hpp"
#include "../inc/dataCenter.hpp"
#include <string>


webserv webs;

std::string rtrim(const std::string &str)
{
    size_t end = str.find_last_not_of(" \t\n\r");

    if (end == std::string::npos)
    {
        return "";
    }
    return str.substr(0, end + 1);
}

void location::isRootSet(server &s)
{
    if (this->_root.empty())
        this->_root = s.getRoot();
}

void fillLocationAttr(std::ifstream &obj, std::string &line, int &nbline, server *s)
{
    location l;
    l.setPath(line, nbline, s);
    while (getline(obj, line) && (rtrim(line).find("server") == std::string::npos))
    {
        nbline++;
        if (line.empty() || is_empty(line.c_str()) || trimStr(line).at(0) == '#')
            continue;

        if (getFirstWord(trimStr(line)) == "dir_listing:")
            l.set_dir_listing(line, nbline);
        else if (getFirstWord(trimStr(line)) == "allow_methods:")
            l.setMethods(line, nbline);
        else if (getFirstWord(trimStr(line)) == "root:")
            l.setRoot(line, nbline);
        else if (getFirstWord(trimStr(line)) == "index:")
            l.setIndexes(line, nbline);
        else if (getFirstWord(trimStr(line)) == "auto_index:")
            l.setAutoIndex(line, nbline);
        else if (getFirstWord(trimStr(line)) == "cgi_path:")
            l.setCgiPath(line, nbline);
        else if (getFirstWord(line) == "upload:")
            l.setUpload(line, nbline);
        else if (getFirstWord(line) == "return:")
        {
            l.setReturn(line, nbline);
        }
        if (trimStr(line).find("- location") != std::string::npos)
        {
//            std::cout << l.getCgiPath().size() << "==" << std::endl;
            l.isRootSet(*s);
            s->addLocation(l);
            checkIndentation(line, 4, nbline);
            fillLocationAttr(obj, line, nbline, s);
            return;
        }
        else if (invalid_directive(trimStr(line), 1))
        {
            throwError("Invalid direction error", nbline);
        }
        else
            checkIndentation(line, 8, nbline);
    }
    nbline++;
//    std::cout << l->getCgiPath().size() << ";"<< std::endl;

    l.isRootSet(*s);
    s->addLocation(l);
//    std::cout << l.getRoot() << "********\n";
//    std::cout << s->getLocations()[0].getCgiPath().size()<< "--"<< std::endl;
//    delete l;
}

void fillServerAttr(std::ifstream &obj, int &nbline)
{
    server s;
    std::string line;
    while (getline(obj, line))
    {
        nbline++;
        if (line.empty() || is_empty(line.c_str()) || trimStr(line).at(0) == '#')
            continue;

        if (getFirstWord(line) == "server_name:")
            s.set_server_name(line, nbline);
        else if (getFirstWord(line) == "listen:")
            s.set_listen(line, nbline);
        else if (getFirstWord(line) == "root:")
            s.setRoot(line, nbline);
        else if (getFirstWord(line) == "error:")
            s.setErrorPages(line, nbline);
        else if (getFirstWord(line) == "max_body_size:")
            s.setMaxBodySize(line, nbline);
        else if (trimStr(line).find("- location") != std::string::npos)
        {
            checkIndentation(line, 4, nbline);
            fillLocationAttr(obj, line, nbline, &s);
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
        if (it->getListen().empty())
            missing("listen", s);
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
        if (line.empty() || is_empty(line.c_str()) || trimStr(line).at(0) == '#')
            continue;
        if (c > 0)
        {
            if (line.at(0) == 32 || line.at(0) == '\t')
                continue;
        }
        if (rtrim(line) == "- server:")
        {
            c++;
            server s;
            checkIndentation(line, 0, nbline);
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
//        location loc = webs.getServers()[0].getLocations()[0];
//        std::cout << loc.getReturn() << "<-" << std::endl;


//        for (std::map<std::string, std::string>::iterator it = loc.getCgiPath().begin(); it != loc.getCgiPath().begin() ; ++it) {
//            std::cout << it->first << ": " << it->second << std::endl;
//            std::cout << "**\n";
//        }
//        std::cout << .get_dir_listing() << std::endl;
//        exit(0);
//        std::cout << webs.getServers().size() << std::endl;
        
        std::cout << webs.getServers()[2].getLocations().size() << '\n';
        // printEntryMsg();
        // dataCenter ds(webs);

        obj.close();
    }
    else
        throw std::runtime_error("Cannot open file.");
}
