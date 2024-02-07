#include "../inc/dataCenter.hpp"
#include <signal.h>
std::string getErrorPath(server& s, int statusCode) {
    std::string path;
    std::map<int, std::string>::iterator it = s.get_error_pages().find(statusCode);
    std::stringstream ss;
    ss << statusCode;

    std::string myString = ss.str();
    if (it != s.get_error_pages().end())
    {
        std::ifstream obj(path.c_str(), std::ios::in);
        if (obj.is_open())
        {
            path = it->second;
            obj.close();
        }
        else
        {
            path = "./Errors/";
            path += myString;
            path += ".html";
        }
    }
    else
    {
        path = "./Errors/";
        path += myString;
        path += ".html";
    }
    return path;
}

std::string getContentTypeFromExtention(std::string ext){
    std::map<std::string, std::string>extention;
    extention["html"] = "text/html";
    extention["htm"] = "text/htm";
    extention["css"] = "text/css";
    extention["xml"] = "text/xml";
    extention["mml"] = "text/mml";
    extention["txt"] = "text/txt";

    extention["gif"] = "image/gif";
    extention["jpeg"] = "image/jpeg";
    extention["png"] = "image/png";
    extention["svg"] = "image/svg+xml";
    extention["x-icon"] = "image/ico";

    extention["json"] = "application/json";
    extention["doc"] = "application/msword";
    extention["pdf"] = "application/pdf";

    extention["mp3"] = "audio/mpeg";
    extention["ogg"] = "audio/ogg";
    extention["x-m4a"] = "audio/m4a";

    extention["mp4"] = "video/mp4";
    extention["mov"] = "video/quicktime";
    extention["webm"] = "video/webm";

    return extention[ext];
}

std::string getHeaderResponse(response res, std::string errorMsg){
    std::ostringstream httpResponse;

    httpResponse << "HTTP/1.1 " << res.getStatusCode() << " " << errorMsg << "\r\n";
    httpResponse << "Content-Type: " << getContentTypeFromExtention(res.getContentType()) << "\r\n";
    httpResponse << "\r\n";
    return httpResponse.str();
}

std::string readBufferFromFile(std::fstream &filePath){
    char buffer[BUFFER_SIZE] = {0};
    filePath.read(buffer, BUFFER_SIZE - 1);
    std::string content(buffer, filePath.gcount());
    return content;
}

void dataCenter::sending(int fd){
    signal(SIGPIPE, SIG_IGN);
    std::map<int , std::string> statusCodeMsgs;
    statusCodeMsgs[200] = "OK";
    statusCodeMsgs[201] = "Created";
    statusCodeMsgs[204] = "No Content";

    statusCodeMsgs[301] = "Moved Permanently";
    
    statusCodeMsgs[400] = "Bad Request";
    statusCodeMsgs[403] = "Forbidden";
    statusCodeMsgs[404] = "Not Found";
    statusCodeMsgs[405] = "Method Not Allowed";
    statusCodeMsgs[408] = "Request TimeOut";
    statusCodeMsgs[409] = "Conflict";
    statusCodeMsgs[413] = "Request Entity Too Large";
    statusCodeMsgs[411] = "Length Required";
    statusCodeMsgs[414] = "Request URI ToLong";

    statusCodeMsgs[500] = "Internal Server Error";
    statusCodeMsgs[501] = "Not Implemented";
    statusCodeMsgs[502] = "Bad Gateway";
    statusCodeMsgs[504] = "Gateway Timeout";
    statusCodeMsgs[505] = "HTTP Version Not Supported";

    response &res = this->clientList[fd].getResponse();
    std::string content = "";
 
    if(!res.getIsHeaderSend()){
        if (res.getStatusCode() == 301)
        {
            std::string header = "HTTP/1.1 301 Moved Permanently\r\nLocation: " + res.getPath() + "\r\nContent-Type: text/html\r\n\r\n";
            write(fd, header.c_str(), header.length());
            close(fd);
            return ;
        }
        if (res.getStatusCode() == 201)
        {
            std::string header = "HTTP/1.1 201 Created\r\nContent-Type: text/html\r\n\r\n";
            // write(fd, header.c_str(), header.length());
            // close(fd);
            // return ;
        }
        if (res.getStatusCode() != 200){
            res.openfilePathError(this->getWebserv().getServers()[this->clientList[fd].servIndx()].get_error_pages()[res.getStatusCode()]);
            if (!res.getFilePathError().is_open())
                res.openfilePathError(getErrorPath(this->getWebserv().getServers()[this->clientList[fd].servIndx()], res.getStatusCode()));
        }
        else{
            res.openFile(res.getPath());
        }
        content = getHeaderResponse(res, statusCodeMsgs[res.getStatusCode()]);
        res.setIsHeaderSend(true);
    }
    else if (res.getLisDir()){
        content = res.getContent();
        res.setLisDir(false);
        res.setIsResponseSent(true);
    }else{
        if (!res.getFilePath().eof() && (res.getStatusCode() == 200 || res.getStatusCode() == 201))
        {
            content = readBufferFromFile(res.getFilePath());
            if (res.getFilePath().eof())
                res.setIsResponseSent(true);
        }
        if (!res.getFilePathError().eof() && res.getStatusCode() != 200)
        {
            content = readBufferFromFile(res.getFilePathError());
            if (res.getFilePathError().eof())
                res.setIsResponseSent(true);
        }

    }
    write(fd, content.c_str(), content.length());
    if (res.getIsResponseSent()){
        std::cout << fd << "()()()()()()()()\n";
        close(fd);
        res.getFilePath().close();
        res.getFilePathError().close();
        if (res.getIsCGIFile()){
            unlink(res.getPath().c_str());
            res.setIsCGIFile(false);
        }
    }

    this->clientList[fd].setResponse(res);
}