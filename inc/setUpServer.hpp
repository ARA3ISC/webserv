#ifndef SETUPSERVER_HPP
# define SETUPSERVER_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <sstream>

#define PORT 8000
//class webserv;
#include "webserv.hpp"
#include "utils3.hpp"



void    startSetUp(webserv& webs);
void acceptClientSocket(int epollfd, int fd, struct epoll_event &ev, struct sockaddr_in &hostAddr,  int host_addrlen);

#endif