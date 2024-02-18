/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dataCenter.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlarabi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 16:04:08 by maneddam          #+#    #+#             */
/*   Updated: 2024/02/18 22:05:09 by rlarabi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/dataCenter.hpp"


dataCenter::dataCenter() {

}
dataCenter::dataCenter(webserv& webs) {
    filePrefix = 0;
    this->wes = webs;
    this->createServerSockets();
    this->createEpoll();
    this->handlingRequests();

}
dataCenter::dataCenter(const dataCenter &rhs) {
    this->wes = rhs.wes;
    this->epollfd = rhs.epollfd;
    this->serv_fds= rhs.serv_fds;
    this->clientList = rhs.clientList;
    this->serversList = rhs.serversList;
}
dataCenter &dataCenter::operator=(const dataCenter &rhs) {
    if (&rhs != this) {
        this->wes = rhs.wes;
        this->epollfd = rhs.epollfd;
        this->serv_fds= rhs.serv_fds;
        this->clientList = rhs.clientList;
        this->serversList = rhs.serversList;
    }
    return *this;
}
dataCenter::~dataCenter() {}


int dataCenter::createSingServSocket(webserv& webs, struct sockaddr_in hostAddr, int i)
{
    int serv_sock;
    char *end;
    int enable = 1;
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    if (setNonBlocking(serv_sock) == -1)
        throw std::runtime_error("Error: non-blocking failed");
    if (serv_sock < 0) {
        throw std::runtime_error("Error creating socket");
    }
    hostAddr.sin_family = AF_INET;
    if (webs.getServers()[i].getListen().size() == 2)
    {
        hostAddr.sin_port = htons(std::strtol(webs.getServers()[i].getListen()[1].c_str(), &end, 10));
        hostAddr.sin_addr.s_addr = inet_addr(webs.getServers()[i].getListen()[0].c_str());
    }
    else {
        hostAddr.sin_port = htons(static_cast<short unsigned int>(std::strtol(webs.getServers()[i].getListen()[0].c_str(), &end, 10)));
        hostAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    bind(serv_sock, reinterpret_cast<struct sockaddr *>(&hostAddr),  sizeof(hostAddr));
    if (listen(serv_sock, SOMAXCONN) < 0)
    {
        throw std::runtime_error("Error listening socket");
    }

    std::string ip = webs.getServers()[i].getListen()[0];
    std::string port = webs.getServers()[i].getListen()[1];
    server s(ip, port);


    this->serversList[i] = s;
    return serv_sock;
}

void dataCenter::createServerSockets() {
    struct sockaddr_in hostAddr;

    for (int i = 0; i < this->wes.get_serverCount(); ++i)
    {
        int sck = createSingServSocket(this->wes, hostAddr, i);
        this->serv_fds.push_back(sck);
    }
}

void dataCenter::createEpoll()
{
    struct epoll_event ev;
    this->epollfd = epoll_create(1);
    if (this->epollfd == -1){
        perror("creating epoll");
    }
    for (unsigned long i = 0; i < this->serv_fds.size(); ++i) {
        ev.events = EPOLLIN;
        ev.data.fd = serv_fds[i];
        if (epoll_ctl(this->epollfd, EPOLL_CTL_ADD, this->serv_fds[i], &ev) == -1)
        {
            throw std::runtime_error("Error epoll ctl");
        }
    }
}

bool dataCenter::isServerFd(std::vector<int> vc, int fd)
{
    if (std::find(vc.begin(), vc.end(), fd) != vc.end())
        return true;
    return false;
}


int dataCenter::setNonBlocking(int sckt) {
    int flag = fcntl(sckt, F_GETFL, 0);
    if (flag == -1)
        return -1;
    flag |= O_NONBLOCK;
    if (fcntl(sckt, F_SETFL, flag))
        return -1;
    return 0;
}

size_t dataCenter::getServerIndex(std::vector<int> s, int fd) {
    std::vector<int>::iterator it = std::find(s.begin(), s.end(), fd);
    size_t indx = std::distance(s.begin(), it );
    return indx;
}

void dataCenter::acceptClientSocket(std::vector<int> server_fds, int fd, struct epoll_event &ev, struct sockaddr_in &hostAddr,  int host_addrlen)
{
    int clientSocket = accept(fd, (struct sockaddr *)&hostAddr, (socklen_t *)&host_addrlen);
    if (clientSocket == -1)
        perror("Error accepting socket");
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = clientSocket;
    if (setNonBlocking(clientSocket) == -1)
        throw std::runtime_error("Error: non-blocking failed");
    if (epoll_ctl(this->epollfd, EPOLL_CTL_ADD, clientSocket,
                  &ev) == -1) {
        perror("epoll_ctl: conn_sock");
    }
    size_t indx = this->getServerIndex(server_fds, fd);
    client c(indx, clientSocket);
    c.setStartTime(clock());

    this->clientList[clientSocket] = c;
    response res;
    this->clientList[clientSocket].setResponse(res);
    std::cout << CYAN << "Client connected " << clientSocket << RESET << std::endl;
}

void dataCenter::handlingRequests()
{
    struct sockaddr_in hostAddr;
    struct epoll_event ev;
    int host_addrlen = sizeof(hostAddr);
    int nfds;
    struct epoll_event events[MAX_EVENTS];
    while (true)
    {
        nfds = epoll_wait(this->epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            throw std::runtime_error("Error epoll wait");
        }
        for (int i = 0; i < nfds; i++)
        {
            if (isServerFd(serv_fds, events[i].data.fd))
            {
                acceptClientSocket(serv_fds, events[i].data.fd, ev, hostAddr, host_addrlen);
            }
            else
            {
                if ((events[i].events & EPOLLIN) && this->clientList[events[i].data.fd].getResponse().getIsReading())
                {
                    this->clientList[events[i].data.fd].setStartTime(clock());


                    try {
                        this->reading(events[i].data.fd);
                    }
                    catch(int){
                    }
                }
                else if ((events[i].events & EPOLLOUT) && !this->clientList[events[i].data.fd].getResponse().getIsReading()) {

                    this->clientList[events[i].data.fd].setStartTime(clock());

                    if (!this->clientList[events[i].data.fd].getResponse().getIsReading())
                    {
                        this->sending(events[i].data.fd);
                    }
                }
                else if (this->clientList[events[i].data.fd].getIsCgiExec()){
                    try
                    {
                        cgi(this->clientList[events[i].data.fd]);
                    }
                    catch(int)
                    {
                        this->clientList[events[i].data.fd].setIsCgiExec(false);
                    }
                }   
                else{

                    if ((clock() - this->clientList[events[i].data.fd].getStartTime() >= 5000000)){
                        this->clientList[events[i].data.fd].setStartTime(clock());
						if (this->clientList[events[i].data.fd].getStartLine().method == "POST"){
							this->clientList[events[i].data.fd].getFileUpload().close();
                        	this->clientList[events[i].data.fd].getResponse().setAttributes(504, "html");
						}
						else{
							close(events[i].data.fd);
							std::cerr << RED << "Response sent [ 504 Gateway Timeout ] " << events[i].data.fd << " Reason: empty request" << RESET << std::endl;
						}
                    }

                }
            }
        }

    }
}

webserv dataCenter::getWebserv(){
    return this->wes;
}

void dataCenter::listDirectory(std::string path, std::string directory, int fd){

    (void)directory;
    DIR* dir;
    struct dirent* ent;
    std::string response = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Directory Listing</title></head><body><h1>Directory Listing</h1><ul>";

    if ((dir = opendir(path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string tmp = ent->d_name;
            if (tmp == ".." || tmp == ".")
                continue;

            if (this->clientList[fd].getStartLine().path == "/")
                response += "<li><a href=\"" + tmp + "\" >" + tmp + "</a></li>";
            else
                response += "<li><a href=\"" + this->clientList[fd].getStartLine().path + tmp + "\" >" + tmp + "</a></li>";
        }
        closedir(dir);
    }
    else
        throw this->clientList[fd].getResponse().setAttributes(403, "html");
    response += "</ul></body></html>";
    this->clientList[fd].getResponse().setAttributes(200, "html");
    this->clientList[fd].getResponse().setContent(response);
    this->clientList[fd].getResponse().setLisDir(true);

    throw 0;
}

std::map<int, server>& dataCenter::getServerList()
{
    return this->serversList;
}

int dataCenter::getFilePrefix(){
    this->filePrefix++;
    return this->filePrefix;
}
