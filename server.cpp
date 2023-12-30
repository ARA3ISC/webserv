#include "server.hpp"
#include "error.hpp"
#include "utils.hpp"


// class members

server::server() {}
server::server(const server& rhs)
{
    this->_locations_count = rhs._locations_count;
    this->_listen = rhs._listen;
    this->_server_name = rhs._server_name;
    this->_root = rhs._root;
    this->_indx = rhs._indx;
    this->_cgi_path = rhs._cgi_path;
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

    if (splited.size() < 2)
        throwError(nbln);
    for (unsigned long i = 1; i < splited.size(); ++i) {
        this->_server_name.push_back(splited[i]);
    }

}

void    server::set_listen(std::string line, int nbln) {
    std::vector<std::string> splited;

    splited = splitBySpace(line);

    if (splited.size() != 2)
        throwError(nbln);
    for (unsigned long i = 1; i < splited.size(); ++i) {
        this->_listen.push_back(splited[i]);
    }
}

void    server::setRoot(std::string line, int nbln){
    std::vector<std::string> splited;

    splited = splitBySpace(line);

    if (splited.size() != 2)
        throwError(nbln);

    this->_root = splited[1];
}

void server::setInndex(std::string line, int nbln)
{
    std::vector<std::string> splited;

    splited = splitBySpace(line);

    if (splited.size() == 1)
        throwError(nbln);
    for (unsigned long i = 1; i < splited.size(); ++i) {
        this->_indx.push_back(splited[i]);
    }
    // this->_index.push_back(splited[1]);
    // std::cout << "=> " << this->_index.size() << std::endl;
}

void    server::setMethods(std::string line, int nbln) {
    std::vector<std::string> splited;

    splited = splitBySpace(line);

    if (splited.size() == 1)
        throwError(nbln);
    for (unsigned long i = 1; i < splited.size(); ++i) {
        this->_allow_methods.push_back(splited[i]);
    }
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
