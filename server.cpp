#include "server.hpp"

server::server() {}
server::server(int locations_count) {
    this->_locations_count = locations_count;
//    this->_locations = new location[locations_count];
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
//    this->_locations = new location[rhs._locations_count];
//    for (int i = 0; i < rhs._locations_count; ++i) {
//        this->_locations[i] = rhs._locations[i];
//    }
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
//        delete[] _locations;
//        this->_locations = new location[rhs._locations_count];
//        for (int i = 0; i < rhs._locations_count; ++i) {
//            this->_locations[i] = rhs._locations[i];
//        }
    }
    return *this;
}

server::~server() {
//    delete[] this->_locations;
}