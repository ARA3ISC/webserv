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
    if (access(path.c_str(), F_OK) != 0)
        return true;
    return false;
}

void dataCenter::cgi(client &clnt,location loc, std::string path, int fd){
    (void)fd;
    server srv = getWebserv().getServers()[clnt.servIndx()];

    if (checkHtmlfile(path))
    {
        std::cout << "CGI file html\n";
        clnt.getResponse().setAttributes(200, "text/html");
        clnt.getResponse().setPath(path);
        throw 0;
    }
    if (checkCgiPaths(loc.getCgiPath()[getExtention(path)])){
        throw clnt.getResponse().setAttributes(502, "text/html");
        // throw returnError(srv, fd, 502);
    }

    int status = 0;
    double timeoutSeconds = 0.0015;
    int pipefd[2];
    if(pipe(pipefd) == -1)
        perror("pipe");
        
    // clock_t start = clock();
    int id = fork();
    if (id == 0){
        const char* programPath = path.c_str();
        char* const argv[] = {(char*)loc.getCgiPath()[getExtention(path)].c_str(), (char*)programPath, NULL};
        char* const envp[] = {NULL};


        close(pipefd[0]);

        dup2(pipefd[1], 1);
        close(pipefd[1]);

        execve(loc.getCgiPath()[getExtention(path)].c_str(), argv, envp);
        exit(127);
    }
    else {
        clock_t start = clock();

        while (waitpid(id, &status, WNOHANG) == 0) {
            
            if (static_cast<double>(clock() - start) / CLOCKS_PER_SEC > timeoutSeconds){
                kill(id, SIGKILL);
                throw clnt.getResponse().setAttributes(504, "text/html");
                // throw returnError(srv, fd, 504);
            }
            usleep(100000);
        }
    }
    // cheking the exist status of child process 
    close(pipefd[1]);

    // sendResponse(fd, 200, "OK", readFileToString(pipefd[0]), "text/html");
    std::cout << "send res 200 from CGI\n";
    clnt.getResponse().setAttributes(200, "text/html");
    clnt.getResponse().setPath(path);
    throw 0;
}