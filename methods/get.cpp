#include "../inc/dataCenter.hpp"
#include <sys/stat.h>

bool fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::string getDefaultError404(){
    std::string err;
    std::string line;
    std::string path = "Errors/404.html";
    std::ifstream input(path.c_str());

    if (input.is_open()){
        while (std::getline(input, line)) {
            err += line;
        }
    }
    return err;
}

std::string getContentFile(std::string path){
    std::string fileContent = "";
    std::string line;
    std::ifstream input(path.c_str());

    if (input.is_open()){
        while (std::getline(input, line)) {
            fileContent += line;
        }
    }
    return fileContent;
}

void sendResponse(int clientSocket, int statusCode, std::string statusMessage, std::string content, std::string contentType) {
    // Construct the HTTP response
    std::ostringstream httpResponse;
    httpResponse << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    httpResponse << "Content-Type: " << contentType << "\r\n";
    httpResponse << "Content-Length: " << content.length() << "\r\n";
    httpResponse << "\r\n";
    httpResponse << content;

    std::cout << "httpResponse : " << httpResponse.str() << std::endl;
    // Send the response back to the client
    write(clientSocket, httpResponse.str().c_str(), httpResponse.str().length());
}

void dataCenter::get(client clnt, int fd){
    server srv = getWebserv().getServers()[clnt.servIndx()];
    std::string path = srv.getLocations()[0].getRoot() + clnt.getStartLine().path ;
    if (!fileExists(path))
    {
        std::cout << "Path not Fount 404\n";
        //content of the file provided in config file
        std::string content = getContentFile(srv.get_error_pages()[404]);
        // in case error file not provivde in config file we get our default file in Errors folder
        if(content == "")
            content = getDefaultError404();
        sendResponse(fd, 404, "Not Found", content, "text/html; charset=utf-8");
    }
    std::cout << clnt.servIndx() << "--" << path << " | " << clnt.getStartLine().path << std::endl;
}