#include "../inc/dataCenter.hpp"

void dataCenter::deleteMethod(client &clnt, int fd)
{
    (void)fd;
    int fdFile;
    std::string directory, file;
    
    server srv = getWebserv().getServers()[clnt.servIndx()];    

    //split the directory and file fron the client request
    // splitPath(clnt.getStartLine().path, directory, file); 
    splitPath(clnt, directory, file); 
    
    //get the index of the location 
    int j = getLocationRequested(srv.getLocations(), directory);

    if (!file.empty()){
        std::string fileName = srv.getLocations()[j].getRoot()+ clnt.getStartLine().path;
        if ((fdFile = open(fileName.c_str(), O_RDONLY)) == -1)
            throw clnt.getResponse().setAttributes(404, "html");
        close(fdFile);
        std::cout << "file to Deleted : " << srv.getLocations()[j].getRoot() << " " << clnt.getStartLine().path << std::endl;
        throw clnt.getResponse().setAttributes(204, "html");
    }
    if (directory == "/"){
        std::cout << "cannot remove root \n";
        throw clnt.getResponse().setAttributes(404, "html");
    }

}