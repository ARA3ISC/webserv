#include "../inc/dataCenter.hpp"

static std::string getErrorPath(server& s, int statusCode) {
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
    // std::cout << statusCode << "=============\n";
    return path;
}

std::string getHeaderResponse(response res, std::string errorMsg){
    std::ostringstream httpResponse;
    // std::cout << "code : " << res.getStatusCode();

    httpResponse << "HTTP/1.1 " << res.getStatusCode() << " " << errorMsg << "\r\n";
    httpResponse << "Content-Type: " << "text/html" << "\r\n";
    // httpResponse << "Content-Length: " << content.length() << "\r\n";
    httpResponse << "\r\n";
    // httpResponse << content;
    return httpResponse.str();
}
std::string readBufferFromFile(std::fstream &filePath){
    // char buffer[1024];
    std::string content;
    filePath.read(&content[0], 1024);
    // std::cout << "content : " << content << std::endl;
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
        
        res.openfilePathError(getErrorPath(this->getWebserv().getServers()[this->clientList[fd].servIndx()], this->clientList[fd].getResponse().getStatusCode()));
        std::cout << res.getFilePathError().is_open() << "`````````````````\n";
        // filePath.open(path.c_str());
        std::cout << "sending header with code : " << statusCodeMsgs[res.getStatusCode()] << " \n";
        content = getHeaderResponse(res, statusCodeMsgs[res.getStatusCode()]);
        res.setIsHeaderSend(true);
    }
    else if (res.getLisDir()){
        std::cout << "list directory\n";
        content = res.getContent();
        res.setLisDir(false);
    }else{
        
        // if (!res.getFilePath().eof()){
        //     std::cout << "lplplplpllp\n";
        //     content = readBufferFromFile(res.getFilePath());
        // }
        if (!res.getFilePathError().eof())
        {
            content = readBufferFromFile(res.getFilePathError());
        }
    
        // std::cout << "read buffer with content : "<< content<<" \n";
    }
    this->clientList[fd].setResponse(res);
    
    if (res.getFilePath().eof() || res.getFilePathError().eof())
    {
        close(fd);
        return;
    }
    write(fd, content.c_str(), content.length());

}