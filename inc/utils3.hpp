#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <cstdio>


bool isServerFd(std::vector<int> vc, int fd);
