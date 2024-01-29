#include "../inc/server.hpp"
#include "../inc/error.hpp"
#include "../inc/dataCenter.hpp"
#include <stdexcept>

bool checkIpSyntax(std::string ip)
{
    std::vector<std::string> tokens = splitBy(ip, '.');
    if (tokens.size() != 4)
        return false;
    for (unsigned long i = 0; i < tokens.size(); i++) {
        if (isNaN(tokens[i]))
            return false;
        else if (std::atoi(tokens[i].c_str()) > 255 || std::atoi(tokens[i].c_str()) < 0)
            return false;
    }
    return true;
}

bool checkListen(std::string ipport)
{
    std::vector<std::string> splited = splitBy(ipport, ':');
    if (splited.size() != 2)
        return false;

    if (!checkIpSyntax(splited[0]) || isNaN(splited[1]))
        return false;
    return true;
}

// class members
server::server() {
    this->max_body_size = 2147483648;
    this->_error_pages[400] = "Errors/400.html";
    this->_error_pages[403] = "Errors/403.html";
    this->_error_pages[404] = "Errors/404.html";
    this->_error_pages[405] = "Errors/405.html";
    this->_error_pages[414] = "Errors/414.html";
    this->_error_pages[500] = "Errors/500.html";
    this->_error_pages[501] = "Errors/501.html";
    this->_error_pages[502] = "Errors/502.html";
    this->_error_pages[504] = "Errors/504.html";
}
server::server(const server& rhs)
{
    this->_locations_count = rhs._locations_count;
    this->_listen = rhs._listen;
    this->_server_name = rhs._server_name;
    this->_root = rhs._root;
    this->_locations = rhs._locations;
    this->_error_pages = rhs._error_pages;
    this->max_body_size = rhs.max_body_size;
}

server &server::operator=(const server &rhs) {
    if (&rhs != this)
    {
        this->_locations_count = rhs._locations_count;
        this->_listen = rhs._listen;
        this->_server_name = rhs._server_name;
        this->_root = rhs._root;
        this->_error_pages = rhs._error_pages;
        this->_locations = rhs._locations;
        this->max_body_size = rhs.max_body_size;
    }
    return *this;
}

void    server::set_server_name(std::string line, int nbln) {
    checkIndentation(line, 4, nbln);

    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);


    if (splited.size() < 2)
        throwError("Syntax error", nbln);
    for (unsigned long i = 1; i < splited.size(); ++i) {
        this->_server_name.push_back(splited[i]);
    }

}

void    server::set_listen(std::string line, int nbln) {
    checkIndentation(line, 4, nbln);

    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);

    if (splited.size() != 2) {
        throwError("Syntax error", nbln);
    }
    if (!this->_listen.empty())
        throwError("Duplicated directive", nbln);

    if (splited[1].find(':') != std::string::npos)
    {
        if (!checkListen(splited[1])) {
            throwError("Syntax error", nbln);
        }
        this->_listen.push_back(splitBy(splited[1], ':')[0]);
        this->_listen.push_back(splitBy(splited[1], ':')[1]);
    }
    else
    {
        if (isNaN(splited[1])) {
            throwError("Syntax error", nbln);
        }
        else {
            this->_listen.push_back(splited[1]);
        }
    }
//    std::cout << "-->" << this->_listen.size() << std::endl;
}

void    server::setRoot(std::string line, int nbln){
    checkIndentation(line, 4, nbln);

    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);


    if (splited.size() != 2)
        throwError("Syntax error", nbln);

    this->_root = splited[1];
}

void server::setErrorPages(std::string line, int nbln)
{
    std::vector<std::string> splited;
    splited = splitBySpace(line);
    removeComment(splited);


    if (splited.size() != 3)
        throwError("Syntax error", nbln);
//    std::cout << "****\n";
    if (!is_digit(splited[1]))
        throwError("Syntax error", nbln);

    std::ifstream obj(splited[2].c_str());
    if (obj.is_open())
    {
        this->_error_pages[std::atoi(splited[1].c_str())] = splited[2];
        std::cout << this->_error_pages[std::atoi(splited[1].c_str())] << "----\n";
        obj.close();
    }
}

void server::setMaxBodySize(std::string line, int nbln) {
//    checkIndentation(line, 4, nbln, s, false);
    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);


    if (splited.size() != 2 || checkMaxBodySize(splited[1]))
    {
        std::cout << splited.size() << "\n";
        throwError("Syntax error", nbln);
    }

//    std::string value = splited[1];
    long  lastResult;
    if (*(splited[1].end() - 1) == 'M' || *(splited[1].end() - 1) == 'm')
        lastResult = getValue(splited[1].c_str()) * 1024 * 1024;
    else if (*(splited[1].end() - 1) == 'G' || *(splited[1].end() - 1) == 'g')
        lastResult = getValue(splited[1].c_str()) * 1024 * 1024 * 1024;
    else if (*(splited[1].end() - 1) == 'K' || *(splited[1].end() - 1) == 'k')
        lastResult = getValue(splited[1].c_str()) * 1024;
    else
        throwError("Invalid unit", nbln);
    this->max_body_size = lastResult;
}

location* server::createLocation()
{
    location* l = new location;
    return l;
}

void server::addLocation(location &location) {
    this->_locations.push_back(location);
    // std::cout << "location created\n";
}

std::vector<location>& server::getLocations()
{
    return this->_locations;
}


server::~server() {
}
