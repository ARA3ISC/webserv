/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsingRequests.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlarabi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 16:04:13 by maneddam          #+#    #+#             */
/*   Updated: 2024/02/19 14:09:35 by rlarabi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/dataCenter.hpp"
#include "../inc/utils2.hpp"
#include "../inc/utils.hpp"
void    dataCenter::requestSyntaxError(client& rq)
{
    std::string uriAllowedCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";

    if (rq.getHeaders().find("Transfer-Encoding") != rq.getHeaders().end()) {
        if (rq.getHeaders().find("Transfer-Encoding")->second != "chunked")
            throw 501;
    }
    if (rq.getHeaders().find("Content-Length") == rq.getHeaders().end() || std::atoi(rq.getHeaders()["Content-Length"].c_str()) == 0){
        if (rq.getStartLine().method == "POST")
            throw 400;
    }

    for (unsigned long i = 0; i < rq.getStartLine().path.size(); ++i) {
        if (uriAllowedCharacters.find(rq.getStartLine().path[i], 0) == std::string::npos)
        {

            throw 400;
        }
    }
    if (rq.getStartLine().path.size() > 2048)
        throw 414;

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
        }
        this->clientList[fd].setbufferBody(trimFromBeginning(this->clientList[fd].getFullRequest(), "\r\n\r\n"));

        requestSyntaxError(this->clientList[fd]);
        this->clientList[fd].headersLoaded(true);
    }
    catch (int e)
    {
        this->clientList[fd].getResponse().setAttributes(e, "html");
        throw 0;
    }

}

void dataCenter::getLocationCF(client &clnt,server srv){

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
        tmpPath  += "/";
        int tmp = getLocationRequested(srv.getLocations(), tmpPath);
        if (tmp != -1)
            indexes.push_back(tmp);
        i++;
    }
        // std::cout << RED << "size index " << indexes.back() << RESET << '\n';
    if (indexes.size() == 0){
        throw clnt.getResponse().setAttributes(404, "html");
    }

    clnt.setLocationIndex(indexes.back());
}

void dataCenter::checkErrors(client &clnt, server srv){

    getLocationCF(clnt, srv);
    // checkPathInfos(clnt.getStartLine().path, clnt);


    if (!srv.getLocations()[clnt.getLocationIndex()].getReturn().empty()){
        clnt.getResponse().setPath(srv.getLocations()[clnt.getLocationIndex()].getReturn());
        throw clnt.getResponse().setAttributes(301, "html");
    }
    if(isMethodAllowed(srv.getLocations()[clnt.getLocationIndex()].getMethods(), clnt.getStartLine().method))
        throw clnt.getResponse().setAttributes(405, "html");



    if (clnt.getStartLine().method == "POST" && srv.getLocations()[clnt.getLocationIndex()].getUpload().empty()){
        throw clnt.getResponse().setAttributes(404, "html");
    }
    if (clnt.getStartLine().method == "POST" && clnt.getHeaders()["Content-Type"].find("boundary") != std::string::npos){
        throw clnt.getResponse().setAttributes(501, "html");
    }
}

int dataCenter::updateServerIndex(server s, std::string hostHeader)
{
    int count = 0;
    std::vector<std::string> splited;
    std::vector<int> repeated;
    splited = splitBy(hostHeader, ':');

    for (size_t i = 0; i < this->wes.getServers().size(); i++)
    {
        if (this->wes.getServers()[i].getListen().size() == 2)
        {
            if (s.getListen()[0] + ':' + s.getListen()[1] == this->wes.getServers()[i].getListen()[0] + ':' + this->wes.getServers()[i].getListen()[1])
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

    if (a == 0 || a == -1)
    {
        struct epoll_event event;

        event.data.fd = fd;
        this->clientList.erase(fd);
        if (epoll_ctl(this->epollfd, EPOLL_CTL_DEL, fd, &event) == -1)
        {
            throw std::runtime_error("Error epoll ctl");
        }
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
            clientList[fd].setServIndx(updateServerIndex(this->getServerList()[clientList[fd].servIndx()], this->clientList[fd].getHeaders()["Host"]));
            checkErrors(this->clientList[fd], this->getWebserv().getServers()[this->clientList[fd].servIndx()]);
        }
        else {
            std::string tmp(buffer, a);
            this->clientList[fd].setbufferBody(tmp);
        }

        if (this->clientList[fd].getStartLine().method == "GET")
            get(this->clientList[fd], fd);

        if (this->clientList[fd].getStartLine().method == "POST")
        {
            post(this->clientList[fd]);
        }
        if (this->clientList[fd].getStartLine().method == "DELETE")
        {
            deleteMethod(this->clientList[fd]);
        }
    }


}
