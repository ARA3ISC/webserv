/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlarabi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 16:00:22 by rlarabi           #+#    #+#             */
/*   Updated: 2024/02/23 20:05:30 by rlarabi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/dataCenter.hpp"

std::string dataCenter::getFileName(std::string pathUpload, std::string directory, std::string extention){
    std::stringstream a;

    a << directory ;
    a << pathUpload;
    a << "/output";
    a << time(0);
    a << this->getFilePrefix();
    a << ".";
    a << extention;

    return a.str();
}

bool isValidHexadecimal(std::string input) {
    if(input.empty())
        return false;
    for (std::string::const_iterator it = input.begin(); it != input.end(); ++it) {
        
        if (!isxdigit(*it)) {
            return false;
        }
    }

    return true;
}

bool checkLastChunk(std::string &hexString){

    if (hexString.find("0\r\n\r\n") != std::string::npos && hexString.find("0\r\n\r\n") == 0)
        return true;
    return false;
}

int hexToDecimal(const std::string& hexString) {
    if (hexString == "" || !isValidHexadecimal(hexString)){
        return 0;
    }
    
    int decimalValue;
    decimalValue = strtoul(hexString.c_str(), NULL, 16);
    
    return decimalValue;
}

int getSizeChunck(std::string &buffer){
    int i = 0;
    std::string hexa;
    if (buffer[i] == '\r' && buffer[i + 1] == '\n'){
        i += 2;
    }
    while((size_t)i < buffer.size() && buffer[i] != '\r'){
        hexa += buffer[i];
        i++;
    }

    if ((size_t)(i + 2) <= buffer.size())
        buffer = buffer.substr(i + 2);

    return hexToDecimal(hexa);
}

void readBufferChunck(client &clnt, std::string buffer){

    size_t i = 0;

    buffer = clnt.getTempBuffer() + buffer ;

    std::string result;
    std::string tmp;

    if (!clnt.getbufferLen()){
        int s = getSizeChunck(buffer);
        clnt.setbufferLen(s);
        clnt.setChunkSize(s);
    }

    int n = clnt.getbufferLen();

    if ((unsigned long)n <= buffer.size()){
        result = buffer.substr(0, n);
        i = n;
        n = 0;
    }else{
        result = buffer;
        i = buffer.size() - 1;
        n -= buffer.size();
    }

    clnt.setbufferLen(n);

    if (!n){
        if (buffer[i] == '\r' && buffer[i + 1] == '\n'){
            i += 2;
        }

        clnt.setTempBuffer(buffer.substr(i));
        std::string tmp = clnt.getTempBuffer();
        
        if (checkLastChunk(tmp)){
            clnt.setIsLastChunk(true);
        }
        clnt.setChunk(result);
        return ;
    }
    else{
        clnt.setTempBuffer("");
        clnt.setChunk(result);
        return ;
    }
}
void dataCenter::checkRealPath(client &clnt, std::string toCheck){

    server srv = getWebserv().getServers()[clnt.servIndx()];
    char realPath[PATH_MAX];
    char currentPath[PATH_MAX];
    char locationRootPath[PATH_MAX];

    realpath(toCheck.c_str(), realPath);
    realpath(srv.getLocations()[clnt.getLocationIndex()].getRoot().c_str(), locationRootPath);
    realpath(".", currentPath);

    std::string s1 = realPath;
    std::string s2 = currentPath;
    std::string s3 = locationRootPath;

    if (s1.find(s2) != std::string::npos && s1 != s2 && s1.find(s3) != std::string::npos){
    }
    else
        throw clnt.getResponse().setAttributes(403, "html");
}

void dataCenter::post(client &clnt){
    
    std::string directory, file, res;
    if (!clnt.getIsUploadfileOpen()){

        server srv = getWebserv().getServers()[clnt.servIndx()];

        splitPath(clnt, directory, file);


        if (file.empty())
            checkRealPath(clnt, directory);
        else
            checkRealPath(clnt, file);

        int j = clnt.getLocationIndex();

        std::size_t lastExtention = clnt.getHeaders()["Content-Type"].find_first_of("/");
        std::string extension = clnt.getHeaders()["Content-Type"].substr(lastExtention + 1);

        std::string fileName;
        
        fileName = getFileName(srv.getLocations()[j].getUpload(), srv.getLocations()[j].getRoot(), extension);

        checkRealPath(clnt, fileName);

        clnt.openFileUpload(fileName);

        clnt.setIsUploadfileOpen(true);

        if (!clnt.getFileUpload().is_open()){
            std::cout << "**\n";
            throw clnt.getResponse().setAttributes(500, "html");
        }

    }
    
    if (clnt.getHeaders()["Transfer-Encoding"] == "chunked" && !clnt.getbufferBody().empty()){

        readBufferChunck(clnt, clnt.getbufferBody());

        if (clnt.getChunk().size() >= clnt.getChunkSize()){
            if (clnt.getChunk().size() == 0 && clnt.getChunkSize() == 0)
            {
                clnt.setIsLastChunk(true);
            }
            clnt.setbufferBody(clnt.getChunk());
            clnt.setChunk("");
            clnt.setChunkSize(0);
        }else{
            return;
        }
    }

    
    if (!clnt.getbufferBody().empty()){
        clnt.setFullSize(clnt.getbufferBody().size());
        clnt.getFileUpload().write(clnt.getbufferBody().c_str(), clnt.getbufferBody().size());

    }
    
    std::istringstream iss(clnt.getHeaders()["Content-Length"]);
    size_t nb;
    iss >> nb;

    if ((clnt.getHeaders()["Transfer-Encoding"] != "chunked" && clnt.getFullSize() >= nb) || (clnt.getHeaders()["Transfer-Encoding"] == "chunked" && clnt.getIsLastChunk())){

        clnt.getFileUpload().close();
        clnt.setFullSize(0);

        splitPath(clnt, directory, file);

        server srv = getWebserv().getServers()[clnt.servIndx()];
        int j = clnt.getLocationIndex();

        if (!file.empty()){
            
            clnt.setFileToCgi(file);
            clnt.setIsPost(1);
            cgi(clnt);
        
        }else{
            if (srv.getLocations()[j].isAutoIndex()){
                std::string fileIndexed;

                if (getContentIndexedFiles(clnt, directory, srv.getLocations()[j].getIndexes(), fileIndexed)){
                
                    if (!checkCgiPaths(srv.getLocations()[j], directory + fileIndexed)){
                        clnt.setFileToCgi(directory + fileIndexed);
                        clnt.setIsPost(1);
                        cgi(clnt);
                    }
                    else
                        throw clnt.getResponse().setAttributes(201, "html");
                }
                else
                    throw clnt.getResponse().setAttributes(201, "html");

            }
            else
                throw clnt.getResponse().setAttributes(201, "html");
        }
    }


}
