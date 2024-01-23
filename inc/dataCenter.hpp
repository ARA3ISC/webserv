#ifndef DATACENTER_H
# define DATACENTER_H

# include <iostream>
# include <string>
# include <arpa/inet.h>
# include <sys/epoll.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <unistd.h>
# define MAX_EVENTS 10
# include "webserv.hpp"
# include "client.hpp"
# define BUFFER_SIZE 1024
#include <dirent.h>
#include <sys/stat.h>
#include "returnError.hpp"

//enum statusCodes {
//    BadRequest = 400,
//    NotFound = 404,
//    NotImplemented = 501,
//    RequestURIToLong = 414,
//    MethodNotAllowed = 405,
//    InternalServerError = 500,
//    Forbidden = 403,
//    RequestTimeOut = 408,
//    Conflict = 409,
//    HTTPVersionNotSupported = 505,
//    PayloadTooLarge = 413,
//    LengthRequired = 411
//};

class dataCenter
{
private:
    webserv wes;
    int epollfd;
    std::vector<int> serv_fds;
    std::map<int, client> clientList;



    /* private member functions */
    int setNonBlocking(int sckt);
    int createSingServSocket(webserv& webs, struct sockaddr_in hostAddr, int i);
    void createEpoll();
    void handlingRequests();
    void createServerSockets();
    bool isServerFd(std::vector<int> vc, int fd);
    void acceptClientSocket(std::vector<int> s, int fd, struct epoll_event &ev, struct sockaddr_in &hostAddr,  int host_addrlen);
    size_t getServerIndex(std::vector<int> s, int fd);

    /* parsing requests functions */
    void    reading(int fd);
    void    loadHeaders(int fd);
    void    startParsingRequest(int fd);
    void    requestSyntaxError(client& rq);

public:
    dataCenter();
    dataCenter(webserv& wes);
    dataCenter(const dataCenter& rhs);
    dataCenter& operator=(const dataCenter& rhs);
    ~dataCenter();
    webserv getWebserv();
    void get(client clnt, int fd);
    void cgi(location loc,std::string path, int fd);
//    const std::string& getErrorPath();


};


#endif
