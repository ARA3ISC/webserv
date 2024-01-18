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

void    dataCenter::startParsingRequest(int fd, std::map<int, client>& mp)
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

void    dataCenter::reading(int fd)
{
    char buffer[BUFFER_SIZE];
    read(fd, buffer, BUFFER_SIZE);
    this->clientList[fd].setFullRequest(buffer);

    if (this->clientList[fd].getFullRequest().find("\r\n\r\n", 0) != std::string::npos)
        startParsingRequest(fd, mp);
}