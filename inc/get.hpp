#ifndef GET_HPP
#define GET_HPP
#include "../inc/dataCenter.hpp"

std::string getContentFile(std::string path);
std::string getDefaultError404();
void Error404(std::string fileName, int fd);
void sendResponse(int clientSocket, int statusCode, std::string statusMessage, std::string content, std::string contentType);

#endif