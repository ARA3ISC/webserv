/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlarabi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 16:00:14 by rlarabi           #+#    #+#             */
/*   Updated: 2024/02/21 21:47:00 by rlarabi          ###   ########.fr       */
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
            closedir(dir);
            return false;
        }
    }
    closedir(dir);
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
        closedir(dir);
    }

}

void dataCenter::deleteMethod(client &clnt)
{

    int fdFile;
    std::string directory, file;

    server srv = getWebserv().getServers()[clnt.servIndx()];

    splitPath(clnt, directory, file);

    if (file.empty())
        checkRealPath(clnt, directory);
    else
        checkRealPath(clnt, file);
    
  
    if (!file.empty()){
        
        if ((fdFile = open(file.c_str(), O_WRONLY)) == -1)
            throw clnt.getResponse().setAttributes(500, "html");
        close(fdFile);
        if (unlink(file.c_str()) == -1)
            throw clnt.getResponse().setAttributes(500, "html");

    }
    else{
        if (pathHasSlashAtEnd(clnt.getStartLine().path))
            throw clnt.getResponse().setAttributes(409, "html");

        deleteDirectory(directory);
        if (!isDirectoryEmpty(directory)){
            throw clnt.getResponse().setAttributes(403, "html");
        }

        DIR *dir = opendir(directory.c_str());
        if (dir != NULL){
            closedir(dir);
            rmdir(directory.c_str());
        }
        else
            throw clnt.getResponse().setAttributes(403, "html");
    }

    throw clnt.getResponse().setAttributes(204, "html");
}
