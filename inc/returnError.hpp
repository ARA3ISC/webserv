#pragma once
#include <iostream>
#include <fstream>
#include "server.hpp"
#include "get.hpp"
/* return error class */
class returnError: public std::exception {
private:
    std::map<int, std::string> statusCodeMsgs;
    // int isHeaderSent;
//    int _statusCode;
//    int _clientFd;
//    int _serverIndx;
public:
    returnError();
    returnError(server& wes, int fd, int statusCode);
    ~returnError() throw();

    const char *what() const throw();

};
