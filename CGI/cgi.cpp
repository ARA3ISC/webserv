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

bool checkCgiPaths(location loc, std::string path){
    std::map<std::string, std::string>::iterator it = loc.getCgiPath().find(getExtention(path));

    if (it != loc.getCgiPath().end())
        return false;
    
    // if (access(path.c_str(), F_OK) != 0)
    //     return true;
    return true;
}

void dataCenter::cgi(client &clnt,location loc, std::string path, int isPost, std::string filePost){
    
    if (checkCgiPaths(loc, path))
    {
        std::cout << "cgi not found\n";
        clnt.getResponse().setAttributes(200,  getExtention(path));
        clnt.getResponse().setPath(path);
        throw 0;
    }
    
    int status = 0;
    double timeoutSeconds = 0.0015;
        
    std::ostringstream s;
    s << std::time(0);
    s << this->getFilePrefix();
    std::string FileName = "/tmp/randomFile" + s.str() + +".txt";
    
    // clnt.getResponse().setIsCGIFile(true);
    
    int fdFile = open(FileName.c_str(), O_CREAT | O_RDWR , 0644);

    if (fdFile == -1){
        std::cout << "error opening tmp file \n";
        throw clnt.getResponse().setAttributes(500, "html");
    }
    int id = fork();
    if (id == 0){
        std::cout << "path : " << path << std::endl;

        const char* programPath = path.c_str();
        char* const argv[] = {(char*)loc.getCgiPath()[getExtention(path)].c_str(), (char*)programPath, NULL};

        std::string queryString = "QUERY_STRING=" + clnt.getQueryString();
        std::string requestMethod = "REQUEST_METHOD=" + clnt.getStartLine().method;
        std::string contentType = "CONTENT_TYPE=" + clnt.getHeaders()["Content-Type"];
        std::string contentLength = "CONTENT_LENGTH=" + clnt.getHeaders()["Content-Length"];
        std::string scriptName = "SCRIPT_NAME=" + clnt.getStartLine().path;
        std::string serverProtocol = "SERVER_PROTOCOL=" + clnt.getStartLine().http_v;

        std::cout << "content Type : " << contentType << std::endl;
        std::cout << "content Length : " << contentLength << std::endl;

        char* const envp[] = {
            (char*)queryString.c_str(),
            (char*)requestMethod.c_str(),
            (char*)contentType.c_str(),
            (char*)contentLength.c_str(),
            (char*)scriptName.c_str(),
            (char*)serverProtocol.c_str(),
            NULL
        };
        int infileFd;
        if (isPost){
            infileFd = open(filePost.c_str(), O_RDWR , 0644);
            
            if (infileFd == -1)
                std::cout << "error opening\n";
            dup2(infileFd, 0);
            close(infileFd);
        }
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
                throw clnt.getResponse().setAttributes(504, "html");
            }
            usleep(100000);
        }
    }
    std::cout << "exit status : " << WEXITSTATUS(status) << std::endl;
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0){
        throw clnt.getResponse().setAttributes(500, "html");
    }

    close(fdFile);

    clnt.getResponse().setAttributes(200, "html");
    clnt.getResponse().setPath(FileName);
    throw 0;
}