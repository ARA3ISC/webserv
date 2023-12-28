#include "webserv.hpp"

webserv::webserv() {}

webserv::webserv(int server_count) {
    this->_serverCount = server_count;
//    this->_servers = new server[server_count];
}

webserv::webserv(const webserv &rhs) {
    this->_serverCount = rhs._serverCount;
    this->_servers = rhs._servers;
//    this->_servers = new server[rhs._serverCount];
//    for (int i = 0; i < rhs._serverCount; ++i) {
//        this->_servers[i] = rhs._servers[i];
//    }
}

webserv& webserv::operator=(const webserv &rhs) {
    if (&rhs != this)
    {
        this->_serverCount = rhs._serverCount;
        this->_servers = rhs._servers;
//        delete[] this->_servers;
//        this->_servers = new server[rhs._serverCount];
//        for (int i = 0; i < rhs._serverCount; ++i) {
//            this->_servers[i] = rhs._servers[i];
//        }
    }
    return *this;
}
webserv::~webserv() {
//    delete[] this->_servers;
}


int webserv::get_serverCount() {
    return this->_serverCount;
}

server* webserv::createServer() {
    server *s = new server;
    return s;
}

void webserv::addServer(server newserv) {
    this->_servers.push_back(newserv);
    // std::cout << "server created\n";
}
