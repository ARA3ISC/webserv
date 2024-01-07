#include "inc/server.hpp"
#include "inc/error.hpp"
#include "inc/utils.hpp"
#include <stdexcept>


// class members
server::server() {
    this->_indx.push_back("index.html");
    this->_indx.push_back("index.htm");
    this->_client_max_body_size = 2000;
}
server::server(const server& rhs)
{
    this->_locations_count = rhs._locations_count;
    this->_listen = rhs._listen;
    this->_server_name = rhs._server_name;
    this->_root = rhs._root;
    this->_indx = rhs._indx;
    this->_cgi_path = rhs._cgi_path;
    this->_error_pages = rhs._error_pages;
    this->_allow_methods = rhs._allow_methods;
    this->_upload = rhs._upload;
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
        this->_indx = rhs._indx;
        this->_cgi_path = rhs._cgi_path;
        this->_error_pages = rhs._error_pages;
        this->_allow_methods = rhs._allow_methods;
        this->_upload = rhs._upload;
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

    if (splited.size() != 2 || isNaN(splited[1]))
        throwError("Syntax error", nbln);

    if (!this->_listen.empty())
        throwError("Duplicated directive", nbln);
    this->_listen = splited[1];
}

void    server::setRoot(std::string line, int nbln){
    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);


    if (splited.size() != 2)
        throwError("Syntax error", nbln);

    this->_root = splited[1];
}

void server::setIndex(std::string line, int nbln)
{
    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);

    if (splited.size() == 1)
        throwError("Syntax error", nbln);
    this->_indx.clear();
    for (unsigned long i = 1; i < splited.size(); ++i) {
        this->_indx.push_back(splited[i]);
    }
    // this->_index.push_back(splited[1]);
    // std::cout << "=> " << this->_index.size() << std::endl;
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

void server::setCgiPath(std::string line, int nbln)
{
    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);


    if (splited.size() != 3)
        throwError("Syntax error", nbln);
    if (invalidCgi(splited[1]))
        throwError("Invalid cgi", nbln);
    if (this->_cgi_path.find(splited[1]) != this->_cgi_path.end())
        throwError("Duplicated symbol", nbln);

    this->_cgi_path.insert(std::pair<std::string, std::string>(splited[1], splited[2]));

}

void server::setMaxBodySize(std::string line, int nbln) {
    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);


    if (splited.size() != 2)
        throwError("Syntax error", nbln);
    this->_client_max_body_size = std::stoi(splited[1]);
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
    if (this->_error_pages.find(std::stoi(splited[1])) != this->_error_pages.end())
    {
        std::cout << "Duplicated key (line: " << nbln << ")";
        throw std::runtime_error("");
    }

    this->_error_pages.insert(std::pair<int, std::string>(std::stoi(splited[1]), splited[2]));
}

void    server::setUpload(std::string line, int nbln)
{
    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);


    if (splited.size() != 2)
        throwError("Syntax error", nbln);
    this->_upload = splited[1];
}

location* server::createLocation()
{
    location* l = new location;
    return l;
}

void server::addLocation(location location) {
    this->_locations.push_back(location);
    // std::cout << "location created\n";
}

std::vector<location>& server::getLocations()
{
    return this->_locations;
}


server::~server() {
}
