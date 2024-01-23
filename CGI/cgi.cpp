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

void cgi(location loc, std::string path, int fd){
    if (checkHtmlfile(path))
    {
        sendResponse(fd, 200, "OK", getContentFile(path), "text/html");
        throw 0;
    }
    // char *cmd = loc.getCgiPath()[getExtention(path)].c_str();
    // char **argv = {}
    int status;
    int id = fork();
    
    // int fileDescriptor = open("./randomfile", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    int pipefd[2];
    if(pipe(pipefd) == -1)
        perror("pipe");

    if (id == 0){
        
        const char* programPath = path.c_str();
        char* const argv[] = {(char*)programPath, NULL};
        char* const envp[] = {NULL};
        
        dup2(pipefd[0], 0);
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        close(pipefd[1]);

        execve(loc.getCgiPath()[getExtention(path)].c_str(), argv, envp);
        std::cout << "EXECVE ERROR\n";
        exit(1);
    }
    waitpid(id, &status, 0);
    while (wait(NULL) != -1)
        ;

    close(pipefd[1]);

    char buffer[100];
    ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer));
    close(pipefd[0]);
    (void)bytesRead;
    // std::string content = getContentFile("./randomfile");

    std::cout << path << " " << loc.getCgiPath()[getExtention(path)] << WEXITSTATUS(status) << "|" << buffer << "|" << std::endl;
    std::cout << "not an html file\n";
}