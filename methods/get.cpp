/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlarabi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 16:00:17 by rlarabi           #+#    #+#             */
/*   Updated: 2024/02/22 18:21:23 by rlarabi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/dataCenter.hpp"


std::string dataCenter::cleanPath(std::string path) {
    std::string result;

    size_t startPos = path.find_first_not_of('/');
    if (startPos != std::string::npos) {
        result = path.substr(startPos);
    }

    size_t endPos = result.find_last_not_of('/');
    if (endPos != std::string::npos) {
        result = result.substr(0, endPos + 1);
    }

    if (!result.empty() && result[0] != '/') {
        result = '/' + result;
    }

    if (result.empty())
        result = "/";
    return result;
}

std::string dataCenter::getCleanPath(std::string path){
    std::size_t last = path.find_last_of("?");
    if (last != std::string::npos)
        return path.substr(0, last);
    return path;
}


int dataCenter::getLocationRequested(std::vector<location> loc, client clnt){

    
    std::vector<std::string> paths;
    for (size_t i = 0; i < loc.size(); i++){
        paths.push_back(loc[i].getPath());
    }
    
    std::sort(paths.begin(), paths.end());

    int j = -1;
    std::string pathStart = clnt.getStartLine().path;
    for (size_t i = 0; i < paths.size(); i++){
        
        if (pathStart[pathStart.size() -1] != '/'){
            pathStart += "/";
        }
            
        if (pathStart.find(paths[i]) != std::string::npos && pathStart.find(paths[i]) == 0)
        {
            j = i;
            break;
        }    
    }
    if (j == -1)
        return -1;
    for(size_t i= 0; i < loc.size(); i++){
        if (paths[j] == loc[i].getPath())
            return i;
    }
    return -1;
}

bool dataCenter::isDirectory(const std::string& path) {
    struct stat fileStat;

    if (stat(path.c_str(), &fileStat) != 0) {
        return false;
    }

    return S_ISDIR(fileStat.st_mode);
}
void removeTrailingSlashes(std::string& str) {
    std::string::size_type pos = str.find_last_not_of('/');

    if (pos != std::string::npos) {
        str.erase(pos + 1);
    }
}

void getQueryStringFromPath(client &clnt, std::string &toOpen){

    size_t pos = toOpen.find_last_of("?");
    if (pos != std::string::npos){
        clnt.setQueryString(toOpen.substr(pos + 1));
        toOpen = toOpen.substr(0, pos);
    }
}

void dataCenter::splitPath(client &clnt,std::string& directory, std::string& file) {
    int index = clnt.getLocationIndex();
    server srv = getWebserv().getServers()[clnt.servIndx()];

    std::string path = srv.getLocations()[index].getPath();
    std::string url = clnt.getStartLine().path;

    std::string trimUrl = trimFromBeginning(url, path);
    if (trimUrl[0] != '/')
        trimUrl.insert(0, "/");
    std::string toOpen = srv.getLocations()[index].getRoot() + trimUrl;

    getQueryStringFromPath(clnt, toOpen);
    
    checkPathInfos(toOpen, clnt);
    size_t pos = toOpen.find(clnt.getPathInfo());
    if (pos != std::string::npos && !clnt.getPathInfo().empty())
        toOpen = toOpen.substr(0, pos);

    if (isDirectory(toOpen)){
        directory = toOpen;
    }else{
        removeTrailingSlashes(toOpen);
        int fd = open(toOpen.c_str(), O_RDONLY);
        if (fd == -1)
        {
            if (!access(toOpen.c_str(), F_OK) && clnt.getStartLine().method == "DELETE")
                throw clnt.getResponse().setAttributes(403, "html");
                
            throw clnt.getResponse().setAttributes(404, "html");
        }
        close(fd);
        file = toOpen;
        return ;
        
    }
}

bool dataCenter::getContentIndexedFiles(client &clnt, std::string path, std::vector<std::string> index, std::string &content)
{
    std::string nameFile;
    server srv = getWebserv().getServers()[clnt.servIndx()];

    for (size_t i = 0; i < index.size(); i++)
    {
        nameFile = path + index[i];
        if (isDirectory(nameFile) && index[i] == srv.getLocations()[clnt.getLocationIndex()].getPath())
            continue;
        if (isDirectory(nameFile) && clnt.getStartLine().method == "GET")
        {
            clnt.getResponse().setPath(index[i]);
            throw clnt.getResponse().setAttributes(301, "html");
        }
        std::ifstream input(nameFile.c_str());
        if (input.is_open())
        {
            content = index[i];
            input.close();
            return true;
        }
        input.close();
    }

    return false;
}

bool dataCenter::isMethodAllowed(std::vector<std::string> methods, std::string method){
    std::vector<std::string>::iterator it = std::find(methods.begin(), methods.end(), method);
    if (it != methods.end())
        return false;
    return true;
}

bool dataCenter::pathHasSlashAtEnd(std::string path){
    if(path.size() != 1 && path[path.size() -1] != '/')
        return true;
    return false;
}

int dataCenter::isPathInfos(std::vector<std::string>& v)
{
    size_t i = 0;
    while(i < v.size()){
        size_t j = 0;
        std::string tmpPath = ".";
        while(j <= i){
            tmpPath += "/" + v[j];
            j++;
        }

        if (!isDirectory(tmpPath))
        {
            return i + 1;
        }
        i++;
    }
    return -1;
}

void dataCenter::checkPathInfos(std::string file, client& clnt)
{
    std::vector<std::string> v = splitBy(file, '/');
    v.erase(v.begin());
    size_t pos = isPathInfos(v);
    std::string p;
    if ((int)pos != -1)
    {
        while (pos < v.size())
        {
            p += "/" + v[pos];
            pos++;
        }
    }
    clnt.setPathInfo(p);
}

void dataCenter::get(client &clnt, int fd){
    std::string directory, file;

    int j = clnt.getLocationIndex();
    server srv = getWebserv().getServers()[clnt.servIndx()];


    splitPath(clnt, directory, file);

    char realPath[PATH_MAX];
    char currentPath[PATH_MAX];
    char locationRootPath[PATH_MAX];

    if (file.empty())
        realpath(directory.c_str(), realPath);
    else
        realpath(file.c_str(), realPath);

    realpath(srv.getLocations()[clnt.getLocationIndex()].getRoot().c_str(), locationRootPath);
    realpath(".", currentPath);
    
    std::string s1 = realPath;
    std::string s2 = currentPath;
    std::string s3 = locationRootPath;
    
    if (s1.find(s2) != std::string::npos && s1 != s2 && s1.find(s3) != std::string::npos){
        
        if (!file.empty())
        {
            clnt.setFileToCgi(file);
            clnt.setIsPost(0);
            cgi(clnt);
            throw 0;
        }
        else
        {
            if (pathHasSlashAtEnd(clnt.getStartLine().path)){
                clnt.getResponse().setPath(clnt.getStartLine().path + "/");
                throw clnt.getResponse().setAttributes(301, "html");
            }

            if (srv.getLocations()[j].isAutoIndex()){
                std::string fileIndexed;

                if (getContentIndexedFiles(clnt, directory, srv.getLocations()[j].getIndexes(), fileIndexed)){
                    clnt.setFileToCgi(directory + fileIndexed);
                    clnt.setIsPost(0);
                    cgi(clnt);
                }
                else if (!srv.getLocations()[j].get_dir_listing())
                    throw clnt.getResponse().setAttributes(403, "html");
            }

            if (!srv.getLocations()[j].get_dir_listing())
                throw clnt.getResponse().setAttributes(403, "html");
            else
                listDirectory(directory , directory, fd);
        }
    }
    else
        throw clnt.getResponse().setAttributes(403, "html");
        
}
