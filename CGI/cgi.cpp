#include "../inc/dataCenter.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


bool checkHtmlfile(std::string file){
    if (file.length() >= 5 && file.substr(file.length() - 5) == ".html")
        return true;
    return false;
}

std::string getExtention(std::string path){
    std::size_t pos = path.find_last_of(".");
    if (pos != std::string::npos)
        return path.substr(pos + 1);
    return "";
}

std::string readFileToString(int fd) {
    std::string content;
    char buffer[1024];

    ssize_t bytesRead;
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        content.append(buffer, bytesRead);
    }
    close(fd);
    return content;
}

bool checkCgiPaths(std::string path){
    std::cout << path << std::endl;
    if (access(path.c_str(), F_OK) != 0)
        return true;
    return false;
}

void dataCenter::cgi(int servIndx ,location loc, std::string path, int fd){
    server srv = getWebserv().getServers()[servIndx];

    if (checkHtmlfile(path))
    {
        sendResponse(fd, 200, "OK", getContentFile(path), "text/html");
        throw 0;
    }
    if (checkCgiPaths(loc.getCgiPath()[getExtention(path)]))
        throw returnError(srv, fd, 502);

    int status;
    
    int pipefd[2];
    if(pipe(pipefd) == -1)
        perror("pipe");
        
    int id = fork();
    if (id == 0){
        
        const char* programPath = path.c_str();
        char* const argv[] = {(char*)loc.getCgiPath()[getExtention(path)].c_str(), (char*)programPath, NULL};
        char* const envp[] = {NULL};


        close(pipefd[0]);

        dup2(pipefd[1], 1);
        close(pipefd[1]);

        execve(loc.getCgiPath()[getExtention(path)].c_str(), argv, envp);
        std::cerr << "EXECVE ERROR\n";
        perror("execve");
        exit(4);
    }
    waitpid(id, &status, 0);
    
    close(pipefd[1]);

    sendResponse(fd, 200, "OK", readFileToString(pipefd[0]), "text/html");

    throw 0;
}