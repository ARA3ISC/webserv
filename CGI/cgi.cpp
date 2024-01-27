#include "../inc/dataCenter.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

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
        clnt.getResponse().setAttributes(200, "text/html");
        clnt.getResponse().setPath(path);
        throw 0;
    }
    if (checkCgiPaths(loc.getCgiPath()[getExtention(path)])){
        throw clnt.getResponse().setAttributes(502, "text/html");
    }
    
    int status = 0;
    double timeoutSeconds = 0.0015;
        
    std::ostringstream s;
    s <<  std::time(0);

    std::string FileName = "/tmp/randomFile" + s.str() + ".txt";
    clnt.getResponse().setIsCGIFile(true);
    
    int fdFile = open(FileName.c_str(), O_CREAT | O_RDWR , 0644);
    
    int id = fork();
    if (id == 0){
        const char* programPath = path.c_str();
        char* const argv[] = {(char*)loc.getCgiPath()[getExtention(path)].c_str(), (char*)programPath, NULL};
        std::string queryString = "QUERY_STRING=" + clnt.getQueryString();
        std::string requestMethod = "REQUEST_METHOD=" + clnt.getStartLine().method;
        
        char* const envp[] = {
            (char*)requestMethod.c_str(),
            (char*)queryString.c_str(),
            NULL
        };

        dup2(fdFile, 1);
        close(fdFile);

        execve(loc.getCgiPath()[getExtention(path)].c_str(), argv, envp);
        exit(127);
    }
    else {
        clock_t start = clock();

        while (waitpid(id, &status, WNOHANG) == 0) {
            
            if (static_cast<double>(clock() - start) / CLOCKS_PER_SEC > timeoutSeconds){
                kill(id, SIGKILL);
                throw clnt.getResponse().setAttributes(504, "text/html");
            }
            usleep(100000);
        }
    }
    // cheking the exist status of child process 
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0){
        throw clnt.getResponse().setAttributes(500, "text/html");
    }

    close(fdFile);

    clnt.getResponse().setAttributes(200, "text/html");
    clnt.getResponse().setPath(FileName);
    throw 0;
}