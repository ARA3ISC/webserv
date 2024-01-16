#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include "server.hpp"

class webserv
{
private:
    int _serverCount;
    std::vector<server> _servers;

public:
    webserv();
    webserv(int server_count);
    webserv(const webserv& rhs);
    webserv& operator=(const webserv& rhs);
    ~webserv();

    int    get_serverCount();
    void    set_serverCount(int i) {
        this->_serverCount = i;
    }
    server* createServer();
    void   addServer(server *newserv);

    std::vector<server>& getServers() {
        return this->_servers;
    }
};

webserv getServer();