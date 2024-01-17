#ifndef SETUPSERVER_HPP
# define SETUPSERVER_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <arpa/inet.h>
#include <sys/epoll.h>
#define PORT 8000
//class webserv;
#include "webserv.hpp"
#include "utils3.hpp"
#define MAX_EVENTS 10
#define BUFFER_SIZE 4096

void    startSetUp(webserv& webs);

#endif