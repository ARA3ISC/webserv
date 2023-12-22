#pragma once

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
    server* _servers;
public:
    webserv();
    webserv(int server_count);
    webserv(const webserv& rhs);
    webserv& operator=(const webserv& rhs);
    ~webserv();

    void    newServer();
    int    get_serverCount();
};
