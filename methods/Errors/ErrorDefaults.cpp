#include "../../inc/get.hpp"

std::string getContentFile(std::string path){
    std::string fileContent = "";
    std::string line;
    std::ifstream input(path.c_str());

    if (input.is_open()){
        while (std::getline(input, line)) {
            fileContent += line;
        }
        input.close();
    }
    return fileContent;
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

void Error404(std::string fileName, int fd){


    //content of the file provided in config file
    std::string content = getContentFile(fileName);
    std::cout << content << std::endl;
    // in case error file not provivde in config file we get our default file in Errors folder
    if(content == "")
        content = getDefaultError404();
    sendResponse(fd, 404, "Not Found", content, "text/html; charset=utf-8");
}
void sendResponse(int clientSocket, int statusCode, std::string statusMessage, std::string content, std::string contentType) {
    // Construct the HTTP response
    std::ostringstream httpResponse;
    httpResponse << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    httpResponse << "Content-Type: " << contentType << "\r\n";
    httpResponse << "Content-Length: " << content.length() << "\r\n";
    httpResponse << "\r\n";
    httpResponse << content;

    // std::cout << "httpResponse : " << httpResponse.str() << std::endl;
    // Send the response back to the client
    write(clientSocket, httpResponse.str().c_str(), httpResponse.str().length());
}