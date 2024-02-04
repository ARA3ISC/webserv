#include "../inc/dataCenter.hpp"
#include "../inc/utils2.hpp"
#include "../inc/utils.hpp"
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
    if (rq.getHeaders().find("Host") == rq.getHeaders().end() || rq.getHeaders()["Content-Type"].find("boundary") != std::string::npos)
        throw 400;

    if (std::atoi(rq.getHeaders()["Content-Length"].c_str()) > this->wes.getServers()[rq.servIndx()].getMaxBodySize())
        throw 413;
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
        this->clientList[fd].setbufferBody(trimFromBeginning(this->clientList[fd].getFullRequest(), "\r\n\r\n"));
        
        // this->clientList[fd].setBody(this->clientList[fd].getFullRequest());
        requestSyntaxError(this->clientList[fd]);
        this->clientList[fd].headersLoaded(true);
    }
    catch (int e)
    {
        this->clientList[fd].getResponse().setAttributes(e, "html");
        throw 0;
        // throw returnError(wes.getServers()[this->clientList[fd].servIndx()], fd, e);
    }

}
void dataCenter::checkErrors(client &clnt, server srv){
    std::string directory, file;
    splitPath(clnt.getStartLine().path, directory, file);

    int j = getLocationRequested(srv.getLocations(), clnt, directory);

    if (!srv.getLocations()[j].getReturn().empty()){
        clnt.getResponse().setPath(srv.getLocations()[j].getReturn());
        throw clnt.getResponse().setAttributes(301, "html");
    }
    if(isMethodAllowed(srv.getLocations()[j].getMethods(), clnt.getStartLine().method))
        throw clnt.getResponse().setAttributes(405, "html");

    std::string path = getCleanPath(srv.getLocations()[j].getRoot() + clnt.getStartLine().path);
    if (!pathExists(path))
        throw clnt.getResponse().setAttributes(404, "html");
    
    if (clnt.getStartLine().method == "POST" && srv.getLocations()[j].getUpload().empty()){
        throw clnt.getResponse().setAttributes(404, "html");
    }
}

void    dataCenter::reading(int fd)
{
    std::string directory, file;

    char buffer[BUFFER_SIZE] = {0};
    int a = read(fd, buffer, BUFFER_SIZE);
    
    if (a == 0)
    {
        // remove the fd from the map
        this->clientList.erase(fd);
        close(fd);
    }
    
    if (!this->clientList[fd].isHeadersLoaded()){
        std::string rqline(buffer, a);
        this->clientList[fd].setFullRequest(rqline);
    }
    if (this->clientList[fd].getFullRequest().find("\r\n\r\n", 0) != std::string::npos)
    {
        if (!this->clientList[fd].isHeadersLoaded())
        {
            loadHeaders(fd);
            checkErrors(this->clientList[fd], this->getWebserv().getServers()[this->clientList[fd].servIndx()]); 
        }
        else {
            std::string tmp(buffer, a);
            this->clientList[fd].setbufferBody(tmp);
        }
        // checking body size with content-length
        
        //checking erorrs of methods

        if (this->clientList[fd].getStartLine().method == "GET")
            get(this->clientList[fd], fd);

        if (this->clientList[fd].getStartLine().method == "POST")
        {
            post(this->clientList[fd], fd);
        }
        // if (this->clientList[fd].getStartLine().method == "DELETE")
        // {
        //     deleteMethod(this->clientList[fd], fd);
        // }
    }


}