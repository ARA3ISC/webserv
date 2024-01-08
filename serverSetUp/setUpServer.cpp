#include "../inc/setUpServer.hpp"
#include "../inc/request.hpp"
#define PORT 7000
#define BUFFER_SIZE 1024

void    startParsingRequest(std::string fullRequest)
{
    std::string line;
    std::istringstream obj(fullRequest);
    request rq;
    getline(obj, line);
    rq.setStartLine(line);

    while(getline(obj, line) && line.at(0) != '\r' && line.at(1) != '\n')
    {
        rq.setHeaders(line);
//        std::cout << line << std::endl;
//        std::cout << rq.getHeaders().begin()->first << std::endl;
    }
//    for (std::map<std::string, std::string>::iterator i = rq.getHeaders().begin(); i != rq.getHeaders().end(); ++i) {
//        std::cout << i->first << " -> " << i->second << std::endl;
//    }
//    std::cout << "method: " << rq.getStartLine().method << "\n" << "path: " << rq.getStartLine().path << "\n" <<
//        "http version: " << rq.getStartLine().http_v << "\n";
}

void    parseRequest(int newFd)
{
    std::string fullRequest;


    char buffer[BUFFER_SIZE];
    read(newFd, buffer, BUFFER_SIZE);
    fullRequest.append(buffer);
    std::cout << fullRequest << std::endl;
    if (fullRequest.find("\r\n\r\n", 0) != std::string::npos)
        startParsingRequest(fullRequest);


}


void    startSetUp() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Creating socket error");
        throw std::runtime_error("");
    }
    std::cout << "Creating server socket ..." << std::endl;

    struct sockaddr_in hostAddr;
    int host_addrlen = sizeof(hostAddr);

    hostAddr.sin_family = AF_INET;
    hostAddr.sin_port = htons(PORT);
    hostAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *) &hostAddr, host_addrlen) != 0) {
        perror("Binding socket error");
        throw std::runtime_error("");
    }
    std::cout << "Binding server socket ..." << std::endl;

    if (listen(sockfd, SOMAXCONN) < 0)
    {
        perror("Error listening socket");
        throw std::runtime_error("");
    }

    for (;;) {
        std::cout << "Server is listening on port " << PORT << std::endl;
        int newSockFd = accept(sockfd, (struct sockaddr *)&hostAddr, (socklen_t *)&host_addrlen);
        if (newSockFd == -1)
        {
            perror("Error accepting socket");
            continue;
        }
        try {
            parseRequest(newSockFd);
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
        close(newSockFd);
    }
}