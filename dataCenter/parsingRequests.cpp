#include "../inc/dataCenter.hpp"

void    dataCenter::requestSyntaxError(client& rq)
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
            throw std::runtime_error("Bad client 400");

    // check URI allowed characters
    for (unsigned long i = 0; i < rq.getStartLine().path.size(); ++i) {
        if (uriAllowedCharacters.find(rq.getStartLine().path[i], 0) == std::string::npos)
            throw std::runtime_error("Bad client 400");
    }
    // check the length of the URI
    if (rq.getStartLine().path.size() > 2048)
        throw std::runtime_error("Request-URI Too Long 414");

    if (rq.getHeaders().find("Host") == rq.getHeaders().end())
        throw std::runtime_error("Bad client 400");

}

void    dataCenter::loadHeaders(int fd)
{
    std::string line;
    std::stringstream obj(this->clientList[fd].getFullRequest());

    getline(obj, line);
    this->clientList[fd].setStartLine(line);

    while (getline(obj, line) && line != "\r") {
        this->clientList[fd].setHeaders(line);
    }
    this->clientList[fd].setBody(this->clientList[fd].getFullRequest());

    requestSyntaxError(this->clientList[fd]);
    this->clientList[fd].headersLoaded(true);
}

void    dataCenter::reading(int fd)
{
    char buffer[BUFFER_SIZE] = {0};
    int a = read(fd, buffer, BUFFER_SIZE);
    if (a == 0)
    {
        if (!this->clientList[fd].getBody().empty())
            std::cout << "[" << this->clientList[fd].getBody() << "]" << std::endl;

//        std::cout << this->clientList[fd].getStartLine().method << std::endl;
        this->clientList.erase(fd);
        close(fd);
        // remove the fd from the map
    }
    this->clientList[fd].setFullRequest(buffer);

    if (this->clientList[fd].getFullRequest().find("\r\n\r\n", 0) != std::string::npos)
    {
        if (!this->clientList[fd].isHeadersLoaded())
            loadHeaders(fd);
        else {
            std::cout << "[" << this->clientList[fd].getBody() << "]" << std::endl;
            // here where u should continue reading from where I stopped
        }
    }
}