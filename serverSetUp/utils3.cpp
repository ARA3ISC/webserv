#include "../inc/utils3.hpp"

bool isServerFd()
{
    if (std::find(serv_fds.begin(), serv_fds.end(), events[i].data.fd) != serv_fds.end())
        return true;
    return false;
}