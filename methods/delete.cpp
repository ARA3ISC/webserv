#include "../inc/dataCenter.hpp"

void dataCenter::deleteMethod(client clnt, int fd)
{
    (void)clnt;
    (void)fd;
    // std::string directory, file;

    // server srv = this->getWebserv().getServers()[clnt.servIndx()];

    // splitPath(clnt.getStartLine().path, directory, file);

    // int j = getLocationRequested(srv.getLocations(), clnt, directory);

    // if (j == -1)
    //     throw returnError(srv, fd, 404);

    // if(isMethodAllowed(srv.getLocations()[j].getMethods(), "DELETE"))
    //     throw returnError(srv, fd, 405);

    // //the complite path of the directory or the file
    // std::string path = getCleanPath(srv.getLocations()[j].getRoot() + clnt.getStartLine().path);

    // //cheking existing of the file on the server

    // if (!pathExists(path))
    // {
    //     throw returnError(srv, fd, 404);
    // }

    // . . .
}