/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlarabi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 15:57:38 by rlarabi           #+#    #+#             */
/*   Updated: 2024/02/25 20:19:25 by rlarabi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/dataCenter.hpp"


std::string getExtention(std::string path){
    std::size_t pos = path.find_last_of(".");
    if (pos != std::string::npos)
        return path.substr(pos + 1);
    return "";
}

bool dataCenter::checkCgiPaths(location loc, std::string path){
    std::map<std::string, std::string>::iterator it = loc.getCgiPath().find(getExtention(path));

    if (it != loc.getCgiPath().end())
        return false;

    return true;
}

void dataCenter::cgi(client &clnt){
    server srv = getWebserv().getServers()[clnt.servIndx()];
    
    checkRealPath(clnt, clnt.getFileToCgi());

    int status;
    
    location loc = this->wes.getServers()[clnt.servIndx()].getLocations()[clnt.getLocationIndex()];
    if (!clnt.getIsCgiExec()){
        
        if (checkCgiPaths(loc, clnt.getFileToCgi()))
        {
            if (clnt.getIsPost()){
                unlink(clnt.getFileUploadName().c_str());
            }
            clnt.getResponse().setAttributes(200,  getExtention(clnt.getFileToCgi()));
            clnt.getResponse().setPath(clnt.getFileToCgi());
            throw 0;
        }
        std::cout << GREEN << "CGI executed [ " << getExtention(clnt.getFileToCgi()) << " ]" << RESET << std::endl;


        std::ostringstream s;
        s << std::time(0);
        s << this->getFilePrefix();
        
        clnt.setFileNameCgi("/tmp/randomFile" + s.str() +".txt");
        clnt.getResponse().setIsCGIFile(true);

        int id = fork();
        if (id == 0){
            
            int infileFd;
            if (clnt.getIsPost()){
                infileFd = open(clnt.getFileUploadName().c_str(), O_RDWR , 0644);
                
                if (infileFd == -1)
                    exit(12);
                if (dup2(infileFd, 0) == -1)
                    exit(12);
                close(infileFd);
            }
            
            int fdFile = open(clnt.getFileNameCgi().c_str(), O_CREAT | O_RDWR , 0644);
            if (fdFile == -1){
                exit(37);
            }
            int logFd = open("./CGI/logfile.log", O_RDWR, 0644);

            std::string pp = clnt.getFileToCgi();
            
            size_t pos = pp.find_last_of("/");
            std::string dir;
            
            if (pos != std::string::npos){
                dir = pp.substr(0, pos);
                
                if (chdir(dir.c_str()) == -1)
                    exit(48);

                pp = pp.substr(pos + 1);
                
            }
            
            
            const char* programPath = pp.c_str();
            char* const argv[] = {(char*)loc.getCgiPath()[getExtention(clnt.getFileToCgi())].c_str(), (char*)programPath, NULL};

            std::string queryString = "QUERY_STRING=" + clnt.getQueryString();
            std::string requestMethod = "REQUEST_METHOD=" + clnt.getStartLine().method;
            std::string contentType = "CONTENT_TYPE=" + clnt.getHeaders()["Content-Type"];
            std::string contentLength = "CONTENT_LENGTH=" + clnt.getHeaders()["Content-Length"];
            std::string scriptName = "SCRIPT_NAME=" + clnt.getStartLine().path;
            std::string serverProtocol = "SERVER_PROTOCOL=" + clnt.getStartLine().http_v;
            std::string redirectStatus = "REDIRECT_STATUS=CGI";
            std::string pathTranslated = "PATH_TRANSLATED=" + pp;
            std::string setCookie = "HTTP_COOKIE=" + clnt.getHeaders()["Cookie"];
            std::string pathInfo = "PATH_INFO=" + clnt.getPathInfo();


            char* const envp[] = {
                (char*)queryString.c_str(),
                (char*)contentType.c_str(),
                (char*)contentLength.c_str(),
                (char*)scriptName.c_str(),
                (char*)serverProtocol.c_str(),
                (char*)redirectStatus.c_str(),
                (char*)requestMethod.c_str(),
                (char*)pathTranslated.c_str(),
                (char*)setCookie.c_str(),
                (char*)pathInfo.c_str(),
                NULL
            };
            
            dup2(fdFile, 1);
            close(fdFile);

            dup2(logFd, 2);
            close(logFd);
            fprintf(stderr, "--------------------------\n");

            execve(loc.getCgiPath()[getExtention(clnt.getFileToCgi())].c_str(), argv, envp);
            perror("execve");
            exit(127);
        }
        
        clnt.setStartTimeCgi(clock());
        clnt.setPidCgi(id);
        clnt.setIsCgiExec(true);

        
    }
    if (static_cast<double>(clock() - clnt.getStartTimeCgi()) / CLOCKS_PER_SEC > 5){
        
        kill(clnt.getPidCgi(), SIGKILL);
        waitpid(clnt.getPidCgi(), &status, 0);
        unlink(clnt.getFileNameCgi().c_str());
        if (clnt.getIsPost())
            unlink(clnt.getFileUploadName().c_str());
        throw clnt.getResponse().setAttributes(504, "html");
    }
    else if(waitpid(clnt.getPidCgi(), &status, WNOHANG) == 0){
        return ;
    }
    
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0){
        std::cout << "exit status : " << WEXITSTATUS(status) << std::endl;
        unlink(clnt.getFileNameCgi().c_str());
        throw clnt.getResponse().setAttributes(500, "html");
    }


    std::fstream input(clnt.getFileNameCgi().c_str());
    char line[1024] = {0};
    input.read(line, 1023);
    std::string tmp = line;
    int i = 0;

    while(true){
        input.read(line, 1023);
        if (input.eof())
            break;
        tmp += line;
        if (i++ > 3 && tmp.find("\r\n\r\n") == std::string::npos)
            break;
    }
    if (tmp.find("\r\n\r\n") != std::string::npos){
        clnt.setIsCgi(true);
    }
    input.close();
    if (clnt.getIsPost())
        unlink(clnt.getFileUploadName().c_str());
    clnt.getResponse().setAttributes(200, "html");
    clnt.getResponse().setPath(clnt.getFileNameCgi());
    throw 0;
}
