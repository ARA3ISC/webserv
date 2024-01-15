#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h> 

std::string httpGET(const std::string& host, const std::string& path) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct hostent* server = gethostbyname(host.c_str());
    if (server == NULL) {
        perror("Error getting host by name");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(80);
    memcpy(&serverAddr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    std::ostringstream request;
    request << "GET " << path << " HTTP/1.1\r\n";
    request << "Host: " << host << "\r\n";
    request << "Connection: close\r\n\r\n";

    if (send(sockfd, request.str().c_str(), request.str().length(), 0) == -1) {
        perror("Error sending request");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    char buffer[4096];
    std::ostringstream response;

    while (true) {
        int bytesRead = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            break;
        }
        buffer[bytesRead] = '\0';
        response << buffer;
    }

    if (response.str().empty()) {
        std::cerr << "Empty response received" << std::endl;
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    close(sockfd);

    return response.str();
}

void sendResponse(int clientSocket, int statusCode, const std::string& statusMessage, const std::string& content) {
    // Construct the HTTP response
    std::ostringstream httpResponse;
    httpResponse << "HTTP/1.1 " << 200 << " " << statusMessage << "\r\n";
    httpResponse << "Content-Type: text/plain\r\n";
    httpResponse << "Content-Length: " << content.length() << "\r\n";
    httpResponse << "\r\n";
    httpResponse << content;

    // Send the response back to the client
    write(clientSocket, httpResponse.str().c_str(), httpResponse.str().length());
}

void handleClient(int clientSocket) {
    char buffer[4096];
    std::string request;

    // Receive the client's request
    while (true) {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            std::cout << request << std::endl;
            break;
        }
        buffer[bytesRead] = '\0';
        request += buffer;

        // Check if the entire request has been received
        if (request.find("\r\n\r\n") != std::string::npos) {
            break;
        }
    }

    // Check if it's a GET request
    if (request.substr(0, 3) == "GET") {
        // Extract the path from the GET request
        size_t pathStart = request.find(' ') + 1;
        size_t pathEnd = request.find(' ', pathStart);
        std::string path = request.substr(pathStart, pathEnd - pathStart);

        // Call httpGET and get the response
        std::string response = httpGET("example.com", path);

        // Send a 200 OK response with the actual response content
        sendResponse(clientSocket, 200, "OK", response);
    } else {
        // Send a 400 Bad Request response for non-GET requests
        sendResponse(clientSocket, 400, "Bad Request", "Invalid request method");
    }

    // Close the client socket
    close(clientSocket);
}
int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Bind failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 10) == -1) {
        perror("Listen failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port 8080..." << std::endl;

    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

        if (clientSocket == -1) {
            perror("Accept failed");
            continue;
        }

        std::cout << "Connection accepted from " << inet_ntoa(clientAddr.sin_addr) << std::endl;

        // Handle the client in a separate function
        handleClient(clientSocket);
    }

    // Close the server socket (never reached in this example)
    close(serverSocket);

    return 0;
}
