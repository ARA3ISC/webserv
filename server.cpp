#include "server.hpp"
#include "error.hpp"
#include <sstream>


// helpful functions

std::vector<std::string> splitBySpace(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> words;
    std::string word;

    while (iss >> word) {
        words.push_back(word);
    }

    return words;
}

// class members

server::server() {}
server::server(int locations_count) {
    this->_locations_count = locations_count;
}
server::server(const server& rhs)
{
    this->_locations_count = rhs._locations_count;
    this->_listen = rhs._listen;
    this->_server_name = rhs._server_name;
    this->_root = rhs._root;
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
        this->_listen.push_back(splited[1]);
    }
}


server::~server() {
}
