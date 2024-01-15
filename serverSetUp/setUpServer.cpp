#include "../inc/setUpServer.hpp"
#include "../inc/request.hpp"



#define BUFFER_SIZE 3000
void    requestSyntaxError(request& rq)
{
    std::string uriAllowedCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";

    /* check transfer encoding value */
    if (rq.getHeaders().find("Transfer-Encoding") != rq.getHeaders().end()) {
        if (rq.getHeaders().find("Transfer-Encoding")->second != "chunked")
            throw std::runtime_error("Not implemented 501");
    }
    else // check absence of content len and transfer encoding and presence of POST method
        if (rq.getHeaders().find("Content-Length") == rq.getHeaders().end())
            if (rq.getStartLine().method == "POST")
                throw std::runtime_error("Bad request 400");

    // check URI allowed characters
    for (unsigned long i = 0; i < rq.getStartLine().path.size(); ++i) {
        if (uriAllowedCharacters.find(rq.getStartLine().path[i], 0) == std::string::npos)
            throw std::runtime_error("Bad request 400");
    }
    // check the length of the URI
    if (rq.getStartLine().path.size() > 2048)
        throw std::runtime_error("Request-URI Too Long 414");

    if (rq.getHeaders().find("Host") == rq.getHeaders().end())
        throw std::runtime_error("Bad request 400");

}

void    startParsingRequest(std::string fullRequest)
{
    std::string line;
    std::istringstream obj(fullRequest);
    request rq;
    getline(obj, line);
    rq.setStartLine(line);

    while(getline(obj, line) && !line.empty() && line.size() != 1 && line != "\r\n" && line != "\n")
        rq.setHeaders(line);
    rq.setBody(fullRequest);
    requestSyntaxError(rq);
//    if (!rq.getBody().empty())
//        std::cout << "|" << rq.getBody() << "|" << std::endl;

//    std::cout << rq.getHeaders().size() << std::endl;
//    for (std::map<std::string, std::string>::iterator i = rq.getHeaders().begin(); i != rq.getHeaders().end(); ++i) {
//        std::cout << "|" << i->first << "| -> " << i->second << std::endl;
//    }
//    std::cout << rq.getHeaders().size() << std::endl;
//    std::cout << "method: " << rq.getStartLine().method << "\n" << "path: " << rq.getStartLine().path << "\n" <<
//        "http version: " << rq.getStartLine().http_v << "\n";
}

void    parseRequest(int newFd)
{
    std::string fullRequest;


    char buffer[BUFFER_SIZE];
    read(newFd, buffer, BUFFER_SIZE);
    fullRequest.append(buffer);

    // std::cout << fullRequest << std::endl;
    if (fullRequest.find("\r\n\r\n", 0) != std::string::npos)
        startParsingRequest(fullRequest);
//    std::cout << "8*******" << std::endl;


}
#include <arpa/inet.h>
void    startSetUp(webserv& webs)
{
    char *end;
    int serv_sock;
    int epollfd;
    std::vector<int> serv_fds;
    struct sockaddr_in hostAddr;
    int host_addrlen = sizeof(hostAddr);

    int enable = 1;
    for (int i = 0; i < webs.get_serverCount(); ++i) {
        /* creating server based on conf file */
        serv_sock = socket(AF_INET, SOCK_STREAM, 0);
        setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
        if (serv_sock < 0) {
            perror("Creating server socket error");
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

        /* binding socket to a port from conf file */
        bind(serv_sock, reinterpret_cast<struct sockaddr *>(&hostAddr), host_addrlen);
        /* Server listens */
        if (listen(serv_sock, SOMAXCONN) < 0)
        {
            perror("Error listening socket");
            throw std::runtime_error("");
        }
        serv_fds.push_back(serv_sock);
    }

    struct epoll_event ev;
    epollfd = epoll_create(1);
    if (epollfd == -1){
        perror("creating epoll");
    }
    for (unsigned long i = 0; i < serv_fds.size(); ++i) {
        ev.events = EPOLLIN;
        ev.data.fd = serv_fds[i];
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, serv_fds[i], &ev) == -1)
        {
            perror("epoll_ctl");
            throw std::runtime_error("Error epoll ctl");
        }
        std::cout << "**********\n";
    }

    int nfds;
    struct epoll_event events[MAX_EVENTS];
    while (true)
    {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            throw std::runtime_error("Error epoll wait");
        }
        for (int i = 0; i < nfds; i++)
        {
            if (std::find(serv_fds.begin(), serv_fds.end(), events[i].data.fd) != serv_fds.end())
                std::cout << "server needed\n";
        }
    }
    accept(serv_fds[1], (struct sockaddr *)&hostAddr, (socklen_t *)&host_addrlen);

    close(serv_fds[1]);


//    exit(0);
//    for (;;) {
//        std::cout << "Server is listening on port " << PORT << std::endl;
//        int newSockFd = accept(sockfd, (struct sockaddr *)&hostAddr, (socklen_t *)&host_addrlen);
//        if (newSockFd == -1)
//        {
//            perror("Error accepting socket");
//            continue;
//        }
//        try {
//            parseRequest(newSockFd);
//        }
//        catch (std::exception& e) {
//            std::cerr << e.what() << std::endl;
//        }
//        close(newSockFd);
//    }
}