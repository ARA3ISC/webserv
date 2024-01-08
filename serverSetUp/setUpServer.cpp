#include "../inc/setUpServer.hpp"
#define PORT 7000
void    startSetUp()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Creating socket error");
        throw std::runtime_error("");
    }
    std::cout << "Creating server socket ..." << std::endl;

    struct sockaddr_in hostAddr;
    int host_addrlen = sizeof(hostAddr);

    hostAddr.sin_family = AF_INET;
    hostAddr.sin_port = htons(PORT);
    hostAddr.sin_addr.s_addr = INADDR_ANY;

//        bind(sockfd, (struct sockaddr_in *)&hostAddr, sizeof(hostAddr));
//        bind
    if (bind(sockfd, (struct sockaddr *)&hostAddr, host_addrlen) != 0)
    {
        perror("Binding socket error");
        throw std::runtime_error("");
    }

    std::cout << "Binding server socket ..." << std::endl;

//    hostAddr

//    hostAddr
}