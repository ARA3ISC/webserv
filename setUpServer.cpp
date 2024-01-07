#include "inc/setUpServer.hpp"

void    startSetUp()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Creating socket error");
        throw std::runtime_error("");
    }
    std::cout << "Server socket created" << std::endl;

//    struct sockaddr_in hostAddr;

//    hostAddr

//    hostAddr
}