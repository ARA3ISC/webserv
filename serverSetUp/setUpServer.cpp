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
    for (int i = 0; i < webs.get_serverCount(); ++i)
    {
        int sck = createSingServSocket(webs, hostAddr, i);
        serv_fds.push_back(sck);
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
        std::cout << "fd of server added to the instance -> " << serv_fds[i] << '\n';
    }

    char r[BUFFER_SIZE];
    int nfds;
    struct epoll_event events[MAX_EVENTS];
    while (true)
    {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            throw std::runtime_error("Error epoll wait");
        }
//        exit(0);
        for (int i = 0; i < nfds; i++)
        {
            int clientSocket;
            if (std::find(serv_fds.begin(), serv_fds.end(), events[i].data.fd) != serv_fds.end())
            {
                clientSocket = accept(events[i].data.fd, (struct sockaddr *)&hostAddr, (socklen_t *)&host_addrlen);
                if (clientSocket == -1)
                {
                    perror("Error accepting socket");
                    continue;
                }
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = clientSocket;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientSocket,
                              &ev) == -1) {
                    perror("epoll_ctl: conn_sock");
                    exit(EXIT_FAILURE);
                }
                read(clientSocket, r, 1024);
                std::cout << r << std::endl;
//                close(clientSocket);
            }
            else
            {
                std::cout << nfds << std::endl;
                close(clientSocket);
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