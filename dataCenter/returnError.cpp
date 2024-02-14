// #include "../inc/returnError.hpp"


// /* implementing the exception class */

// // std::string getErrorPath(server& s, int statusCode) {
// //     std::string path;
// //     std::map<int, std::string>::iterator it = s.get_error_pages().find(statusCode);
// //     std::stringstream ss;
// //     ss << statusCode;

// //     std::string myString = ss.str();
// //     if (it != s.get_error_pages().end())
// //     {
// //         std::ifstream obj(path.c_str());
// //         if (obj.is_open())
// //         {
// //             path = it->second;
// //             obj.close();
// //         }
// //         else
// //         {
// //             path = "./Errors/";
// //             path += myString;
// //             path += ".html";
// //         }
// //     }
// //     else
// //     {
// //         path = "./Errors/";
// //         path += myString;
// //         path += ".html";
// //     }
// //     return path;
// // }

// returnError::returnError() {}
// returnError::returnError(server& wes, int fd, int statusCode) {
//     this->statusCodeMsgs[400] = "Bad Request";
//     this->statusCodeMsgs[404] = "Not Found";
//     this->statusCodeMsgs[501] = "Not Implemented";
//     this->statusCodeMsgs[502] = "Bad Gateway";
//     this->statusCodeMsgs[414] = "Request URI ToLong";
//     this->statusCodeMsgs[405] = "Method Not Allowed";
//     this->statusCodeMsgs[500] = "Internal Server Error";
//     this->statusCodeMsgs[403] = "Forbidden";
//     this->statusCodeMsgs[408] = "Request TimeOut";
//     this->statusCodeMsgs[409] = "Conflict";
//     this->statusCodeMsgs[505] = "HTTP Version Not Supported";
//     this->statusCodeMsgs[504] = "Gateway Timeout";
//     this->statusCodeMsgs[413] = "Payload Too Large";
//     this->statusCodeMsgs[411] = "Length Required";


//     (void)wes;
//     // std::string path = getErrorPath(wes, statusCode);

//     // std::cout << "path: " << path << std::endl;

//     std::string content;
//     // content = getContentFile(path);
//     std::ostringstream httpResponse;


//     httpResponse << "HTTP/1.1 " << statusCode << " " << this->statusCodeMsgs[statusCode] << "\r\n";
//     httpResponse << "Content-Type: " << "text/html" << "\r\n";
//     httpResponse << "Content-Length: " << content.length() << "\r\n";
//     httpResponse << "\r\n";
//     httpResponse << content;

// //    std::cout << "last fd " << fd << std::endl;
//     write(fd, httpResponse.str().c_str(), httpResponse.str().length());
// }

// const char *returnError::what() const throw() {
//     return "";
// }
// returnError::~returnError() throw() {}