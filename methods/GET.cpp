/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GET.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlarabi <rlarabi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:56:32 by rlarabi           #+#    #+#             */
/*   Updated: 2024/01/16 18:17:14 by rlarabi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"
#include "../inc/request.hpp"
#include <unistd.h>
#include <string>
#include <fstream>
#include <dirent.h>

void sendResponse(int clientSocket, int statusCode, std::string statusMessage, std::string content, std::string contentType) {
    // Construct the HTTP response
    std::ostringstream httpResponse;
    httpResponse << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    httpResponse << "Content-Type: " << contentType << "\r\n";
    httpResponse << "Content-Length: " << content.length() << "\r\n";
    httpResponse << "\r\n";
    httpResponse << content;



    std::cout << "httpResponse : " << httpResponse.str() << std::endl;
    // Send the response back to the client
    write(clientSocket, httpResponse.str().c_str(), httpResponse.str().length());
}
std::string getUrlPath(std::string path){
    
    size_t lastSlash = path.find_last_of('/');
    
    if (lastSlash != std::string::npos)
        path = path.substr(0, lastSlash);
    return path;
}
bool checkMatchingLocations(std::string path){
    std::vector<server> serv = getServer().getServers();
    std::vector<location> loc = serv[0].getLocations();
    std::vector<location>::iterator it = loc.begin();
    while(it != loc.end()){
        std::string str = it->getPath();
        str.erase(str.size() - 1);
        std::cout << "paths of the server : " << str << " and " << path << std::endl;
        if(str == (path))
            return true;
        it++;
    }
    return false;
}

std::string getContent(std::ifstream &input){
    std::string buffer;
    std::string content;
    while(getline(input, buffer)){
        content += buffer;
    }
    return content;
}

std::string Error404(){
    std::string err;
    err = "<!DOCTYPE html>\n";
    err += "<html lang=\"en\">\n";
    err += "<head>\n";
    err += "    <meta charset=\"UTF-8\">\n";
    err += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    err += "    <title>Document</title>\n";
    err += "</head>\n";
    err += "<body>\n";
    err += "    <div> Error 404 </div>\n";
    err += "</body>\n";
    err += "</html>\n";
    return err;
}

std::string DefaultResponse(){
    std::string res;
    res = "<!DOCTYPE html>\n";
    res += "<html lang=\"en\">\n";
    res += "<head>\n";
    res += "    <meta charset=\"UTF-8\">\n";
    res += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    res += "    <title>Document</title>\n";
    res += "</head>\n";
    res += "<body>\n";
    res += "    <form action=\"\" method=\"get\">\n";
    res += "        <input type=\"text\" name=\"text\" id=\"\">\n";
    res += "        <input type=\"submit\" value=\"submit\">\n";
    res += "    </form>\n";
    res += "</body>\n";
    res += "</html>\n";
    return res;
}

std::vector<location>::iterator get_Location(std::vector<location> loc, std::string path){
    std::vector<location>::iterator it = loc.begin();
    
    while(it != loc.end()){
        if(it->getPath() == path)
            return it;
        it++;
    }
    return it;
}
std::string getPath(std::string path){
     size_t lastSlash = path.find_last_of('/');

    std::string fileName = path.substr(lastSlash + 1);

    size_t firstQuestionMark = fileName.find_first_of('?');

    if (firstQuestionMark != std::string::npos) {
        fileName = fileName.substr(0, firstQuestionMark);
    }
    return fileName;
}
void GET(int sockfd, request *req){
    std::vector<server> serv = getServer().getServers();
    std::vector<location> loc = serv[0].getLocations();
    std::vector<location>::iterator locIt;
    
    std::string path = "myWebsite" + req->getStartLine().path;
    
    if (!checkMatchingLocations(getUrlPath(path)) && path.find(".html") == std::string::npos){
        sendResponse(sockfd, 404, "Not Found", Error404(), "text/html; charset=utf-8");
        return ;
    }
    if (path.find(".html") != std::string::npos){
        path = getUrlPath(path) + "/" + getPath(path);
        if (path[0] == '.')
            path.insert(0, ".");
        std::ifstream input(path.c_str());
        if(!input.is_open())
        {
            sendResponse(sockfd, 404, "Not Found", Error404(), "text/html; charset=utf-8");
            return ;
        }
        sendResponse(sockfd, 200, "OK", getContent(input), "text/html; charset=utf-8");
        return ;
    }
    else
    {
        // locIt = get_Location(loc, path);
        
        // check if auto index
        // std::cout << locIt->getIndex();
        
        path.insert(0, ".");
        DIR* dir = opendir(path.c_str());
        std::string res;
        if (dir != NULL) {
            struct dirent* entry;
            res = "<html><body><h1>Directory Listing</h1><ul>";
            while ((entry = readdir(dir)) != NULL) {
                std::string tmp = entry->d_name;
                res += "<a href=\"" + path + "/" + tmp + "\">" + tmp + "</a><br>";
            }
            res += "</ul></body></html>";
            closedir(dir);
            sendResponse(sockfd, 200, "OK", res, "text/html; charset=utf-8");
            return ;
        }
        
    }

    sendResponse(sockfd, 200, "OK", DefaultResponse(), "text/html; charset=utf-8");
}

    // std::vector<server> serv = getServer().getServers();
    // std::vector<location> loc = serv[0].getLocations();
    // std::string path = loc[0].getRoot();
    // path.erase(path.size() - 1);
    // path += req->getStartLine().path;