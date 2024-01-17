#ifndef DATACENTER_H
# define DATACENTER_H

//# include "parsingConfigFile.hpp"
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#define BUFFER_SIZE 1024
#define MAX_EVENTS 10
# include "webserv.hpp"


class dataCenter
{
private:
    webserv wes;
    int epollfd;
    std::vector<int> serv_fds;

    int setNonBlocking(int sckt);
    int createSingServSocket(webserv& webs, struct sockaddr_in hostAddr, int i);
    void createEpoll();
    void handlingRequests();
    void createServerSockets();
    bool isServerFd(std::vector<int> vc, int fd);
    void acceptClientSocket(int fd, struct epoll_event &ev, struct sockaddr_in &hostAddr,  int host_addrlen);

public:
    dataCenter();
    dataCenter(webserv& wes);
    dataCenter(const dataCenter& rhs);
    dataCenter& operator=(const dataCenter& rhs);
    ~dataCenter();
};

#endif
