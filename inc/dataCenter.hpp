/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dataCenter.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlarabi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 16:04:34 by maneddam          #+#    #+#             */
/*   Updated: 2024/02/19 22:51:01 by rlarabi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
# include "webserv.hpp"
# include "client.hpp"
# include <dirent.h>
# include <sys/stat.h>
# include "utils3.hpp"
# include <ctime>
# include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
# define MAX_EVENTS 10
# define BUFFER_SIZE 2048
# define GREEN "\033[92m"
# define RESET "\033[0m"
# define RED "\033[31m"
# define CYAN "\033[96m"


class dataCenter
{
private:
    webserv wes;
    int epollfd;
    std::vector<int> serv_fds;
    std::map<int, client> clientList;
    int filePrefix;
    std::map<int, server> serversList;


    int setNonBlocking(int sckt);
    int createSingServSocket(webserv& webs, struct sockaddr_in hostAddr, int i);
    void createEpoll();
    void handlingRequests();
    void createServerSockets();
    bool isServerFd(std::vector<int> vc, int fd);
    void acceptClientSocket(std::vector<int> s, int fd, struct epoll_event &ev, struct sockaddr_in &hostAddr,  int host_addrlen);
    size_t getServerIndex(std::vector<int> s, int fd);

    void    reading(int fd);
    void    sending(int fd);
    void    loadHeaders(int fd);
    void    requestSyntaxError(client& rq);

public:
    dataCenter();
    dataCenter(webserv& wes);
    dataCenter(const dataCenter& rhs);
    dataCenter& operator=(const dataCenter& rhs);
    ~dataCenter();
    webserv getWebserv();

    bool isMethodAllowed(std::vector<std::string> methods, std::string method);;
    std::string cleanPath(std::string path);
    std::string getCleanPath(std::string path);
    void splitPath(client &clnt, std::string& directory, std::string& file);
    int getLocationRequested(std::vector<location> loc, client clnt);

    int updateServerIndex(server s, std::string hostHeader);
    void get(client &clnt, int fd);
    void post(client &clnt);
    void cgi(client &clnt);
    void deleteMethod(client &clnt);



    void listDirectory(std::string path, std::string directory, int fd);
    void checkErrors(client &clnt, server serv);
    std::string getFileName(std::string pathUpload, std::string directory, std::string extention);
    int getFilePrefix();
    bool isDirectory(const std::string& path);
    void getLocationCF(client &clnt,server srv);
    void deleteDirectory(std::string directory);
    bool pathHasSlashAtEnd(std::string path);
    bool checkCgiPaths(location loc, std::string path);
    bool getContentIndexedFiles(std::string path, std::vector<std::string> index,std::string &content);

int isPathInfos(std::vector<std::string>& v);
void checkPathInfos(std::string file, client& clnt);

    std::map<int, server>& getServerList() ;
};


#endif
