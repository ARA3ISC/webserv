#ifndef WEBSERV_HPP
# define WEBSERV_HPP

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
    server* createServer();
    void   addServer(server &newserv);
    void    set_serverCount(int i);
    std::vector<server>& getServers();


};

#endif
