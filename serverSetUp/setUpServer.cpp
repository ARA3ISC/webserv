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

    while(getline(obj, line) && line != "\r\n" && !line.empty() && line[0] != '\n')
    {
        std::cout << line << std::endl;
        rq.setHeaders(line);
    }
    rq.setBody(fullRequest);

//    std::map<std::string, std::string>::iterator it = rq.getHeaders().begin();
//    std::cout << "|" << it->first <<"|" << std::endl;

    requestSyntaxError(rq);
//    if (!rq.getBody().empty())
//        std::cout << "body:\n" << rq.getBody() << "\n" << std::endl;

//    std::cout << rq.getHeaders().size() << std::endl;
//    for (std::map<std::string, std::string>::iterator i = rq.getHeaders().begin(); i != rq.getHeaders().end(); ++i) {
//        std::cout << "|" << i->first << "| -> " << i->second << std::endl;
//    }
    std::cout << rq.getHeaders().size() << std::endl;
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
//    if (fullRequest.find("\r\n\r\n", 0) != std::string::npos)
        startParsingRequest(fullRequest);
//    std::cout << "8*******" << std::endl;


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