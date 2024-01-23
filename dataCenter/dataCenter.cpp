/* Canonical form */
#include "../inc/dataCenter.hpp"


dataCenter::dataCenter() {

}
dataCenter::dataCenter(webserv& webs) {
    this->wes = webs;
    this->createServerSockets();
    this->createEpoll();
    this->handlingRequests();
}
dataCenter::dataCenter(const dataCenter &rhs) {
    this->wes = rhs.wes;
    this->epollfd = rhs.epollfd;
    this->serv_fds= rhs.serv_fds;
    this->clientList = rhs.clientList;
}
dataCenter &dataCenter::operator=(const dataCenter &rhs) {
    if (&rhs != this) {
        this->wes = rhs.wes;
        this->epollfd = rhs.epollfd;
        this->serv_fds= rhs.serv_fds;
        this->clientList = rhs.clientList;
    }
    return *this;
}
dataCenter::~dataCenter() {}

/* end of canonical form */

int dataCenter::createSingServSocket(webserv& webs, struct sockaddr_in hostAddr, int i)
{
    int serv_sock;
    char *end;
    int enable = 1;
    /* creating server based on conf file */
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    if (setNonBlocking(serv_sock) == -1)
        throw std::runtime_error("Error: non-blocking failed");
    if (serv_sock < 0) {
        perror("Creating server socket error");
        throw std::runtime_error("Error creating socket");
    }
    hostAddr.sin_family = AF_INET;
    if (webs.getServers()[i].getListen().size() == 2)
    {
        hostAddr.sin_port = htons(std::strtol(webs.getServers()[i].getListen()[1].c_str(), &end, 10));
        hostAddr.sin_addr.s_addr = inet_addr(webs.getServers()[i].getListen()[0].c_str());
    }
    else {
        hostAddr.sin_port = htons(static_cast<short unsigned int>(std::strtol(webs.getServers()[i].getListen()[0].c_str(), &end, 10)));
        hostAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    /* binding socket to a port from conf file */
    bind(serv_sock, reinterpret_cast<struct sockaddr *>(&hostAddr),  sizeof(hostAddr));
    /* Server listens */
    if (listen(serv_sock, SOMAXCONN) < 0)
    {
        perror("Error listening socket");
        throw std::runtime_error("");
    }
    return serv_sock;
}

void dataCenter::createServerSockets() {
    struct sockaddr_in hostAddr;

    /* Creating and binding and listening servers sockets based on config file + saving servers sockets that will be added to the epoll instance  */
    for (int i = 0; i < this->wes.get_serverCount(); ++i)
    {
        int sck = createSingServSocket(this->wes, hostAddr, i);
        this->serv_fds.push_back(sck);
    }
}

void dataCenter::createEpoll()
{
    struct epoll_event ev;
    this->epollfd = epoll_create(1);
    if (this->epollfd == -1){
        perror("creating epoll");
    }
    for (unsigned long i = 0; i < this->serv_fds.size(); ++i) {
        ev.events = EPOLLIN;
        ev.data.fd = serv_fds[i];
        if (epoll_ctl(this->epollfd, EPOLL_CTL_ADD, this->serv_fds[i], &ev) == -1)
        {
            perror("epoll_ctl");
            throw std::runtime_error("Error epoll ctl");
        }
    }
}

bool dataCenter::isServerFd(std::vector<int> vc, int fd)
{
    if (std::find(vc.begin(), vc.end(), fd) != vc.end())
        return true;
    return false;
}


int dataCenter::setNonBlocking(int sckt) {
    int flag = fcntl(sckt, F_GETFL, 0);
    if (flag == -1)
        return -1;
    flag |= O_NONBLOCK;
    if (fcntl(sckt, F_SETFL, flag))
        return -1;
    return 0;
}

size_t dataCenter::getServerIndex(std::vector<int> s, int fd) {
    std::vector<int>::iterator it = std::find(s.begin(), s.end(), fd);
    size_t indx = std::distance(s.begin(), it );
//    std::cout << indx << "---------\n";
    return indx;
}

void dataCenter::acceptClientSocket(std::vector<int> server_fds, int fd, struct epoll_event &ev, struct sockaddr_in &hostAddr,  int host_addrlen)
{
    int clientSocket = accept(fd, (struct sockaddr *)&hostAddr, (socklen_t *)&host_addrlen);
    if (clientSocket == -1)
        perror("Error accepting socket");
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = clientSocket;
    if (setNonBlocking(clientSocket) == -1)
        throw std::runtime_error("Error: non-blocking failed");
    if (epoll_ctl(this->epollfd, EPOLL_CTL_ADD, clientSocket,
                  &ev) == -1) {
        perror("epoll_ctl: conn_sock");
    }
//    std::cout << "****\n";
//    std::cout << "client Socket: " << clientSocket << std::endl;
    size_t indx = this->getServerIndex(server_fds, fd);
    client c(indx, clientSocket);
    
    this->clientList[clientSocket] = c;
    
//    std::cout << clientSocket << " " << c.servIndx() << " " << this->clientList[clientSocket].servIndx() << std::endl;

}

void dataCenter::handlingRequests()
{
    struct sockaddr_in hostAddr;
    struct epoll_event ev;
    int host_addrlen = sizeof(hostAddr);
    int nfds;
    struct epoll_event events[MAX_EVENTS];
    while (true)
    {
        nfds = epoll_wait(this->epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            throw std::runtime_error("Error epoll wait");
        }
        for (int i = 0; i < nfds; i++)
        {
            /* check if the fd is a server fd */
            if (isServerFd(serv_fds, events[i].data.fd))
            {
                acceptClientSocket(serv_fds, events[i].data.fd, ev, hostAddr, host_addrlen);
            }
            else
            {
                if (events[i].events & EPOLLIN)
                {
                    try {
                        this->reading(events[i].data.fd);
                    }
                    catch (returnError& e) {
                        (void)e;
//                        std::cout << "hahahaha\n";
//                        std::cout << "first fd " << events[i].data.fd << std::endl;

                        close(events[i].data.fd);
                    }
                }
//                else if (events[i].events & EPOLLOUT) {}
                /* check if the response is ready to send */
            }
        }

    }
}

webserv dataCenter::getWebserv(){
    return this->wes;
}
