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

    int status = 0;
    double timeoutSeconds = 0.0015;
    int pipefd[2];
    if(pipe(pipefd) == -1)
        perror("pipe");
        
    // clock_t start = clock();
    int id = fork();
    if (id == 0){
        std::cout << "child\n";
        const char* programPath = path.c_str();
        char* const argv[] = {(char*)loc.getCgiPath()[getExtention(path)].c_str(), (char*)programPath, NULL};
        char* const envp[] = {NULL};


        close(pipefd[0]);

        dup2(pipefd[1], 1);
        close(pipefd[1]);

        execve(loc.getCgiPath()[getExtention(path)].c_str(), argv, envp);
        throw returnError(srv, fd, 500);
        perror("execve");
        exit(12);
    }
    else {
        clock_t start = clock();

        while (waitpid(id, &status, WNOHANG) == 0) {
            
            if (static_cast<double>(clock() - start) / CLOCKS_PER_SEC > timeoutSeconds){
                kill(id, SIGKILL);
                throw returnError(srv, fd, 504);
            }
            usleep(100000);
        }
        if (WIFEXITED(status)) {
            std::cout << "Child process exited with status: " << WEXITSTATUS(status) << std::endl;
        } else {
            std::cout << "Child process did not exit normally." << std::endl;
        }

        // return 0;
        std::cout << "end\n";
    }
    // std::cout << "parent\n";
    // clock_t start = clock();

    // while (1) {
    //     std::cout <<  static_cast<double>(clock() - start) / CLOCKS_PER_SEC << " " << WEXITSTATUS(status) << std::endl;
    //     if(static_cast<double>(clock() - start) / CLOCKS_PER_SEC > timeoutSeconds){
    //         close(pipefd[1]);
    //         throw returnError(srv, fd, 504);
    //     }
    //     std::cout << "wautpid : " << waitpid(id, &status, 1) << std::endl;
    //     if (waitpid(id, &status, 1) == 0){
    //         break;
    //     }
    //     usleep(100000);  // Sleep for 100 milliseconds
    // }
    
    close(pipefd[1]);
    // waitpid(id, &status, 0);

    sendResponse(fd, 200, "OK", readFileToString(pipefd[0]), "text/html");
    std::cout << "send res 200";
    throw 0;
}