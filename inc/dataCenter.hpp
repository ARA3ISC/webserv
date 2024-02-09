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
# define BUFFER_SIZE 2048
#include <dirent.h>
#include <sys/stat.h>
#include "returnError.hpp"
#include "utils3.hpp"
#include <ctime>
#include <cstring>

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
    int filePrefix;


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
    void    sending(int fd);
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

    /* common functions */
    bool isMethodAllowed(std::vector<std::string> methods, std::string method);;
    std::string cleanPath(std::string path);
    std::string getCleanPath(std::string path);
    void splitPath(client &clnt, std::string& directory, std::string& file);
    int getLocationRequested(std::vector<location> loc, std::string path);
    bool pathExists(const std::string& path);

    int updateServerIndex(std::string host);
        /* get method*/
    void get(client &clnt, int fd);
    void post(client &clnt, int fd);
    void cgi(client &clnt ,location loc,std::string path, int isPost, std::string filePost);
    void deleteMethod(client &clnt, int fd);
//    const std::string& getErrorPath();


    /* delete method */

    void listDirectory(std::string path, std::string directory, int fd);
    void checkErrors(client &clnt, server serv); 
    std::string getFileName(std::string pathUpload, std::string directory, std::string extention);
    int getFilePrefix();
    bool isDirectory(const std::string& path);
    void getLocationCF(client &clnt,server srv);
    void deleteDirectory(std::string directory);

};


#endif
