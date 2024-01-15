#ifndef SETUPSERVER_HPP
# define SETUPSERVER_HPP

#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <sys/epoll.h>
#define PORT 8000
//class webserv;
#include "webserv.hpp"

void    startSetUp(webserv& webs);

#endif