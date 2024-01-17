#include "../inc/setUpServer.hpp"
#include "../inc/request.hpp"

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

void    startParsingRequest(int fd, std::map<int, request>& mp)
{
//    std::cout << mp[fd].getFullRequest() << std::endl;
    std::string line;
    std::stringstream obj(mp[fd].getFullRequest());
    getline(obj, line);
    mp[fd].setStartLine(line);

//    std::cout << "[" << mp[fd].getStartLine().http_v << "]";
    while(getline(obj, line) && line != "\r")
    {
        std::cout << "size : " <<  line.size() << std::endl;
        mp[fd].setHeaders(line);
    }
//    mp[fd].setBody(mp[fd].getFullRequest());
//    requestSyntaxError(mp[fd]);
}

void    reading(int fd, std::map<int, request>& mp)
{
    char buffer[BUFFER_SIZE];
    read(fd, buffer, BUFFER_SIZE);
    mp[fd].setFullRequest(buffer);


    if (mp[fd].getFullRequest().find("\r\n\r\n", 0) != std::string::npos)
        startParsingRequest(fd, mp);
}

int createSingServSocket(webserv& webs, struct sockaddr_in hostAddr, int i)
{
    int serv_sock;
    char *end;
    int enable = 1;
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
    bind(serv_sock, reinterpret_cast<struct sockaddr *>(&hostAddr),  sizeof(hostAddr));
    /* Server listens */
    if (listen(serv_sock, SOMAXCONN) < 0)
    {
        perror("Error listening socket");
        throw std::runtime_error("");
    }
    return serv_sock;
}

void    startSetUp(webserv& webs)
{
    int epollfd;
    std::vector<int> serv_fds;
    struct sockaddr_in hostAddr;
    int host_addrlen = sizeof(hostAddr);

    /* Creating and binding and listening servers sockets based on config file + saving servers sockets that will be added to the epoll instance  */
//    std::cout << "Webserv is listening on ports : ";
    for (int i = 0; i < webs.get_serverCount(); ++i)
    {
        int sck = createSingServSocket(webs, hostAddr, i);
        serv_fds.push_back(sck);
//        std::cout << webs.getServers()[i].getListen()[0] << " ";
    }
//    std::cout << "\n";
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
    }

//    char r[BUFFER_SIZE];
    int nfds;
    struct epoll_event events[MAX_EVENTS];
    std::map<int, request> mapClientFds;
    while (true)
    {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            throw std::runtime_error("Error epoll wait");
        }
        for (int i = 0; i < nfds; i++)
        {
            /* check if the if the fd is a server fd */
            if (std::find(serv_fds.begin(), serv_fds.end(), events[i].data.fd) != serv_fds.end())
            {
                int clientSocket = accept(events[i].data.fd, (struct sockaddr *)&hostAddr, (socklen_t *)&host_addrlen);
                if (clientSocket == -1)
                {
                    perror("Error accepting socket");
                    continue;
                }
                ev.events = EPOLLIN | EPOLLOUT;
                ev.data.fd = clientSocket;
                request rq;
                mapClientFds[clientSocket] = rq;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientSocket,
                              &ev) == -1) {
                    perror("epoll_ctl: conn_sock");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                if (events[i].events & EPOLLIN)
                {
//                    request rq = mapClientFds.at(events[i].data.fd);
//                    Client c = map.find(fd);
//                    reading(events[i].data.fd, mapClientFds);
                    char r[BUFFER_SIZE];
//                    std::string pp;
                    size_t a;
                    do {
                        a = read(events[i].data.fd, r, sizeof(r));
                        if (a > 0) {
                            // Ensure that the null character is appended within the valid range
                            if (a < sizeof(r)) {
                                r[a] = '\0';
                            } else {
                                // Handle the case where the buffer is full
                                // or take appropriate action based on your requirements
                            }
                        }
                        std::cout <<r << std::endl;
                        std::cout << "*";
                    } while (a > 0);

//                    std::cout << mapClientFds[events[i].data.fd].getStartLine().method << std::endl;
                    close(events[i].data.fd);

                    exit(0);
                }
            }
        }

    }



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