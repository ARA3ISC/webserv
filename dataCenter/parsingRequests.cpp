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
    if (rq.getHeaders().find("Content-Length") == rq.getHeaders().end() || std::atoi(rq.getHeaders()["Content-Length"].c_str()) == 0){
        if (rq.getStartLine().method == "POST")
            throw 400;
    }
    
    // check URI allowed characters
    for (unsigned long i = 0; i < rq.getStartLine().path.size(); ++i) {
        if (uriAllowedCharacters.find(rq.getStartLine().path[i], 0) == std::string::npos)
        {
            
            throw 400;
        }
    }
    // check the length of the URI
    if (rq.getStartLine().path.size() > 2048)
        throw 414;

    /* if no host present in the headers*/
    if (rq.getHeaders().find("Host") == rq.getHeaders().end())
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
// location /home1
// location /home1/a1 --> url : /home1/a1/b1/file.php

void dataCenter::getLocationCF(client &clnt,server srv){

    // std::cout << 
    std::string reqUrl = clnt.getStartLine().path;
    if (reqUrl.find_last_of("?") != std::string::npos)
        reqUrl = reqUrl.substr(0, reqUrl.find_last_of("?"));
    std::vector<std::string> splitURL = splitBy(reqUrl, '/');
    std::vector<int> indexes;
    size_t i = 0;

    int tmp = getLocationRequested(srv.getLocations(), "/");
    if (tmp != -1)
        indexes.push_back(tmp);

    while(i < splitURL.size()){
        size_t j = 0;
        std::string tmpPath;
        while(j <= i){
            tmpPath += "/" + splitURL[j];
            j++;
        }
        
        int tmp = getLocationRequested(srv.getLocations(), tmpPath);
        if (tmp != -1)
            indexes.push_back(tmp);
        i++;
    }
    if (indexes.size() == 0){
        throw clnt.getResponse().setAttributes(404, "html");
    }

    clnt.setLocationIndex(indexes.back());

}


void dataCenter::checkErrors(client &clnt, server srv){
    std::string directory, file;
    
    getLocationCF(clnt, srv);

    if (!srv.getLocations()[clnt.getLocationIndex()].getReturn().empty()){
        clnt.getResponse().setPath(srv.getLocations()[clnt.getLocationIndex()].getReturn());
        throw clnt.getResponse().setAttributes(301, "html");
    }
    if(isMethodAllowed(srv.getLocations()[clnt.getLocationIndex()].getMethods(), clnt.getStartLine().method))
        throw clnt.getResponse().setAttributes(405, "html");

    // std::string path = getCleanPath(srv.getLocations()[clnt.getLocationIndex()].getRoot() + clnt.getStartLine().path);
    // if (!pathExists(path))
    //     throw clnt.getResponse().setAttributes(404, "html");
    
    if (clnt.getStartLine().method == "POST" && srv.getLocations()[clnt.getLocationIndex()].getUpload().empty()){
        throw clnt.getResponse().setAttributes(404, "html");
    }
}

int dataCenter::updateServerIndex(std::string host)
{
    int count = 0;
    std::vector<std::string> splited;
    std::vector<int> repeated;
    splited = splitBy(host, ':');

    for (size_t i = 0; i < this->wes.getServers().size(); i++)
    {
        if (this->wes.getServers()[i].getListen().size() == 1)
        {
            /* one port in config file && host port matches this port */
            if (this->wes.getServers()[i].getListen()[0] == splited[1])
                return i;
        }
        if (this->wes.getServers()[i].getListen().size() == 2)
        {
            if (host == this->wes.getServers()[i].getListen()[0] + ':' + this->wes.getServers()[i].getListen()[1])
            {
                repeated.push_back(i);
                count++;
            }
        }
    }

    if (count > 1)
    {
        for (size_t i = 0; i < repeated.size(); i++)
        {
            for (size_t k = 0; k < this->wes.getServers()[repeated[i]].getServer_names().size(); k++)
            {
                if (splited[0] == this->wes.getServers()[repeated[i]].getServer_names()[k])
                {
                    return repeated[i];
                }
            }   
        }
    }
    if (repeated.size() == 0)
        repeated.push_back(0);
    return repeated[0];
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
            clientList[fd].setServIndx(updateServerIndex(this->clientList[fd].getHeaders()["Host"]));
            // std::cout << clientList[fd].servIndx() << '\n';
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
        if (this->clientList[fd].getStartLine().method == "DELETE")
        {
            deleteMethod(this->clientList[fd], fd);
        }
    }


}