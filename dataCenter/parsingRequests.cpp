#include "../inc/dataCenter.hpp"

void    dataCenter::requestSyntaxError(client& rq)
{
    std::string uriAllowedCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";

    /* check transfer encoding value */
    if (rq.getHeaders().find("Transfer-Encoding") != rq.getHeaders().end()) {
        if (rq.getHeaders().find("Transfer-Encoding")->second != "chunked")
            throw 501;
    }
    else // check absence of content len and transfer encoding and presence of POST method
        if (rq.getHeaders().find("Content-Length") == rq.getHeaders().end())
            if (rq.getStartLine().method == "POST")
                throw 400;

    // check URI allowed characters
    for (unsigned long i = 0; i < rq.getStartLine().path.size(); ++i) {
        if (uriAllowedCharacters.find(rq.getStartLine().path[i], 0) == std::string::npos)
            throw 400;
    }
    // check the length of the URI
    if (rq.getStartLine().path.size() > 2048)
        throw 414;

    /* if no host present in the headers*/
    if (rq.getHeaders().find("Host") == rq.getHeaders().end())
        throw 400;


}

void    dataCenter::loadHeaders(int fd)
{
    std::string line;
    std::stringstream obj(this->clientList[fd].getFullRequest());

    getline(obj, line);
    try {
        this->clientList[fd].setStartLine(line);

        while (getline(obj, line) && line != "\r") {
            this->clientList[fd].setHeaders(line);
//        std::cout << this->clientList[fd].getHeaders().begin()->first << std::endl;
        }
        this->clientList[fd].setBody(this->clientList[fd].getFullRequest());
        requestSyntaxError(this->clientList[fd]);
        this->clientList[fd].headersLoaded(true);
    }
    catch (int e)
    {
        this->clientList[fd].getResponse().setAttributes(e, "text/html");
        throw 0;
        // throw returnError(wes.getServers()[this->clientList[fd].servIndx()], fd, e);
    }

}
#include <cstring>

void    dataCenter::reading(int fd)
{

    char buffer[BUFFER_SIZE] = {0};
    int a = read(fd, buffer, BUFFER_SIZE - 1);
    if (a == 0)
    {
        this->clientList.erase(fd);
        close(fd);
        // remove the fd from the map
    }
    
    std::string rqline(buffer, a);
    
    this->clientList[fd].setFullRequest(rqline);

    if (this->clientList[fd].getFullRequest().find("\r\n\r\n", 0) != std::string::npos)
    {

        if (!this->clientList[fd].isHeadersLoaded())
        {
            loadHeaders(fd);
        }
        else {
            this->clientList[fd].setBody(this->clientList[fd].getFullRequest());
        }
        // checking body size with content-length
        
        if (this->clientList[fd].getStartLine().method == "GET")
        {
            get(this->clientList[fd], fd);
        }
        if (this->clientList[fd].getStartLine().method == "POST")
        {
            // std::cout << this->clientList[fd].getFullRequest() << std::endl;
            // std::cout << "^^" << std::atoi(this->clientList[fd].getHeaders()["Content-Length"].c_str()) << std::endl;
            // std::cout << "^^" << this->clientList[fd].getBody().size() << std::endl;
            // if (std::atoi(this->clientList[fd].getHeaders()["Content-Length"].c_str()) == this->clientList[fd].get)
            // std::cout << "post request\n";
            if (this->clientList[fd].getBody().size() == static_cast<std::size_t>(std::atoi(this->clientList[fd].getHeaders()["Content-Length"].c_str())))
                post(this->clientList[fd], fd);
        }
        // if (this->clientList[fd].getStartLine().method == "DELETE")
        // {
        //     deleteMethod(this->clientList[fd], fd);
        // }
    }


}