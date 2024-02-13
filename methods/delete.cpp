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
    if (pathHasSlashAtEnd(clnt.getStartLine().path))
        throw clnt.getResponse().setAttributes(409, "html");

    int fdFile;
    std::string directory, file;

    server srv = getWebserv().getServers()[clnt.servIndx()];

    splitPath(clnt, directory, file);


    if (!file.empty()){
        if ((fdFile = open(file.c_str(), O_RDONLY)) == -1)
            throw clnt.getResponse().setAttributes(404, "html");
        close(fdFile);
        if (unlink(file.c_str()) == -1)
            throw clnt.getResponse().setAttributes(500, "html");

    }
    else{
        char realPath[PATH_MAX];
        char currentPath[PATH_MAX];

        realpath(directory.c_str(), realPath);
        realpath(".", currentPath);

        std::string s1 = realPath;
        std::string s2 = currentPath;

        if (s1.find(s2) != std::string::npos && s1 != s2){

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
        else
            throw clnt.getResponse().setAttributes(403, "html");

    }
    throw clnt.getResponse().setAttributes(204, "html");
}
