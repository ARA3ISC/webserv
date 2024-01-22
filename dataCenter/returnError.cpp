#include "../inc/returnError.hpp"


/* implementing the exception class */

std::string getErrorPath(server& s, int statusCode) {
    std::string path;
    std::map<int, std::string>::iterator it = s.get_error_pages().find(statusCode);
    std::stringstream ss;
    ss << statusCode;
    std::string myString = ss.str();
    if (it != s.get_error_pages().end())
        path = it->second;
    else
    {
        path = "./Errors/";
        path += myString;
        path += ".html";
    }
    return path;
}

returnError::returnError() {}
returnError::returnError(server& wes, int fd, int statusCode) {
    this->statusCodeMsgs[400] = "BadRequest";
    this->statusCodeMsgs[404] = "NotFound";
    this->statusCodeMsgs[501] = "NotImplemented";
    this->statusCodeMsgs[414] = "RequestURIToLong";
    this->statusCodeMsgs[405] = "MethodNotAllowed";
    this->statusCodeMsgs[500] = "InternalServerError";
    this->statusCodeMsgs[403] = "Forbidden";
    this->statusCodeMsgs[408] = "RequestTimeOut";
    this->statusCodeMsgs[409] = "Conflict";
    this->statusCodeMsgs[505] = "HTTPVersionNotSupported";
    this->statusCodeMsgs[413] = "PayloadTooLarge";
    this->statusCodeMsgs[411] = "LengthRequired";



    std::string path = getErrorPath(wes, statusCode);
    std::string content;
    content = getContentFile(path);
    std::ostringstream httpResponse;


    httpResponse << "HTTP/1.1 " << statusCode << " " << this->statusCodeMsgs[statusCode] << "\r\n";
    httpResponse << "Content-Type: " << "text/html" << "\r\n";
    httpResponse << "Content-Length: " << content.length() << "\r\n";
    httpResponse << "\r\n";
    httpResponse << content;

//    std::cout << "last fd " << fd << std::endl;
    write(fd, httpResponse.str().c_str(), httpResponse.str().length());
}

const char *returnError::what() const throw() {
    return "";
}
returnError::~returnError() throw() {}