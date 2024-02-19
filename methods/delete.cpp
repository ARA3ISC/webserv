/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlarabi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 16:00:14 by rlarabi           #+#    #+#             */
/*   Updated: 2024/02/19 23:29:50 by rlarabi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/dataCenter.hpp"

bool isDirectoryEmpty(std::string directory){

    DIR* dir = opendir(directory.c_str());
    struct dirent* ent;
    bool empty = true;
    if(dir == NULL)
        return false;
    while((ent = readdir(dir)) != NULL){
        std::string tmp = ent->d_name;
        if (tmp != "." && tmp != ".."){
            return false;
        }
    }
    return empty;
}

void dataCenter::deleteDirectory(std::string directory){

    DIR* dir;
    struct dirent* ent;

    if ((dir = opendir(directory.c_str())) != NULL){

        while((ent = readdir(dir)) != NULL){

            std::string tmp = ent->d_name;
            std::string toDelete = directory + "/" + tmp;
            if (tmp == "." || tmp == "..")
                continue;

            if (isDirectory(toDelete)){
                deleteDirectory(toDelete);
            }
            else{
                int fd = open(toDelete.c_str(), O_WRONLY);
                if (fd != -1){
                    close(fd);
                    unlink(toDelete.c_str());
                }
            }
            if (isDirectory(toDelete) && isDirectoryEmpty(toDelete)){
                rmdir(toDelete.c_str());
                continue;
            }
        }
    }

}

void dataCenter::deleteMethod(client &clnt)
{

    int fdFile;
    std::string directory, file;

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

        if (!file.empty()){
            file = s1;
            
            if ((fdFile = open(file.c_str(), O_WRONLY)) == -1)
                throw clnt.getResponse().setAttributes(500, "html");
            close(fdFile);
            if (unlink(file.c_str()) == -1)
                throw clnt.getResponse().setAttributes(500, "html");

        }
        else{
            if (pathHasSlashAtEnd(clnt.getStartLine().path))
                throw clnt.getResponse().setAttributes(409, "html");

            directory = s1;
            deleteDirectory(directory);
            if (!isDirectoryEmpty(directory)){
                throw clnt.getResponse().setAttributes(403, "html");
            }

            DIR *dir = opendir(directory.c_str());
            if (dir != NULL)
                rmdir(directory.c_str());
            else
                throw clnt.getResponse().setAttributes(403, "html");
        }
    }
    else
        throw clnt.getResponse().setAttributes(403, "html");

    throw clnt.getResponse().setAttributes(204, "html");
}
