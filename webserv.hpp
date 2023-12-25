#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include "server.hpp"

//typedef struct s_location {
//    std::string _path;
//    bool _dir_listing;
//    std::vector<std::string> _allow_methods;
//    std::string _root;
//    std::string _index;
//    bool _auto_index;
//}       t_location;
//
//typedef struct s_server {
//    int _locations_count;
//    std::vector<std::string> _listen;
//    std::vector<std::string> _server_name;
//    std::string _root;
//    std::map<std::string, std::string> _cgi_path;
//    std::vector<std::string> _allow_methods;
//    std::vector<std::string> _error_pages;
//    std::string _upload;
//    location* _locations;
//}       t_server;

class webserv
{
private:
    int _serverCount;
    std::vector<server> _servers;
//    server* _servers;
//    std::vector<t_server> _servers;
public:
    webserv();
    webserv(int server_count);
    webserv(const webserv& rhs);
    webserv& operator=(const webserv& rhs);
    ~webserv();

    int    get_serverCount();
    server* createServer();
    void   addServer(server* newserv);
};
