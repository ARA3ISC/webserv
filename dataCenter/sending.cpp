#include "../inc/dataCenter.hpp"

std::string getErrorPath(server& s, int statusCode) {
    std::string path;
    std::map<int, std::string>::iterator it = s.get_error_pages().find(statusCode);
    std::stringstream ss;
    ss << statusCode;

    std::string myString = ss.str();
    if (it != s.get_error_pages().end())
    {
        std::ifstream obj(path.c_str());
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

std::string getHeaderResponse(response res, std::string errorMsg){
    std::ostringstream httpResponse;

    httpResponse << "HTTP/1.1 " << res.getStatusCode() << " " << errorMsg << "\r\n";
    httpResponse << "Content-Type: " << "text/html" << "\r\n";
    // httpResponse << "Content-Length: " << content.length() << "\r\n";
    httpResponse << "\r\n";
    // httpResponse << content;
    return httpResponse.str();
}

std::string readBufferFromFile(std::fstream &filePath){
    char buffer[1024] = {0};
    std::string content;
    filePath.read(buffer, 1023);
    content = buffer;
    return content;
}

void dataCenter::sending(int fd){
    std::map<int , std::string> statusCodeMsgs;
    statusCodeMsgs[200] = "OK";
    statusCodeMsgs[400] = "Bad Request";
    statusCodeMsgs[404] = "Not Found";
    statusCodeMsgs[501] = "Not Implemented";
    statusCodeMsgs[502] = "Bad Gateway";
    statusCodeMsgs[414] = "Request URI ToLong";
    statusCodeMsgs[405] = "Method Not Allowed";
    statusCodeMsgs[500] = "Internal Server Error";
    statusCodeMsgs[403] = "Forbidden";
    statusCodeMsgs[408] = "Request TimeOut";
    statusCodeMsgs[409] = "Conflict";
    statusCodeMsgs[505] = "HTTP Version Not Supported";
    statusCodeMsgs[504] = "Gateway Timeout";
    statusCodeMsgs[413] = "Payload Too Large";
    statusCodeMsgs[411] = "Length Required";

    response &res = this->clientList[fd].getResponse();
    std::string content = "";
    
    if(!res.getIsHeaderSend()){
        if (res.getStatusCode() != 200)
            res.openfilePathError(getErrorPath(this->getWebserv().getServers()[this->clientList[fd].servIndx()], this->clientList[fd].getResponse().getStatusCode()));
        else
            res.openFile(res.getPath());
        content = getHeaderResponse(res, statusCodeMsgs[res.getStatusCode()]);
        res.setIsHeaderSend(true);
    }
    else if (res.getLisDir()){
        content = res.getContent();
        res.setLisDir(false);
        res.setIsResponseSent(true);
    }else{
        if (!res.getFilePath().eof() && res.getStatusCode() == 200)
        {
            if (!res.getContent().empty()){
                content = res.getContent();
                res.setIsResponseSent(true);
            }else{
                content = readBufferFromFile(res.getFilePath());
                if (res.getFilePath().eof())
                    res.setIsResponseSent(true);
            }
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
        std::cout << "()()()()()()()()\n";
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