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
    this->_client_max_body_size = 2000;
}
server::server(const server& rhs)
{
    this->_locations_count = rhs._locations_count;
    this->_listen = rhs._listen;
    this->_server_name = rhs._server_name;
    this->_root = rhs._root;
    this->_error_pages = rhs._error_pages;
    this->_allow_methods = rhs._allow_methods;
    this->_client_max_body_size = rhs._client_max_body_size;
    this->_locations = rhs._locations;
}

server &server::operator=(const server &rhs) {
    if (&rhs != this)
    {
        this->_locations_count = rhs._locations_count;
        this->_listen = rhs._listen;
        this->_server_name = rhs._server_name;
        this->_root = rhs._root;
        this->_error_pages = rhs._error_pages;
        this->_allow_methods = rhs._allow_methods;
        this->_client_max_body_size = rhs._client_max_body_size;
        this->_locations = rhs._locations;
    }
    return *this;
}

void    server::set_server_name(std::string line, int nbln) {
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
    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);


    if (splited.size() != 2)
        throwError("Syntax error", nbln);

    this->_root = splited[1];
}

void    server::setMethods(std::string line, int nbln) {
    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);

    if (splited.size() == 1)
        throwError("Syntax error", nbln);
    if (invalidMethod(splited))
    {
        std::cout << "Invalid method (line: " << nbln << ")";
        throw std::runtime_error("");
    }
    if (hasDuplicates(splited))
        throwError("Duplicated symbol", nbln);
    this->_allow_methods.clear();
    for (unsigned long i = 1; i < splited.size(); ++i) {
        this->_allow_methods.push_back(splited[i]);
    }
}

void server::setMaxBodySize(std::string line, int nbln) {
    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);


    if (splited.size() != 2)
        throwError("Syntax error", nbln);
    this->_client_max_body_size = std::atoi(splited[1].c_str());
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
    if (this->_error_pages.find(std::atoi(splited[1].c_str())) != this->_error_pages.end())
    {
        std::cout << "Duplicated key (line: " << nbln << ")";
        throw std::runtime_error("");
    }

    this->_error_pages.insert(std::pair<int, std::string>(std::atoi(splited[1].c_str()), splited[2]));
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
