#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

const int PORT = 8000;

void handleRequest(int clientSocket) {
    // Read the HTTP request from the client
    char buffer[1024];
    std::stringstream request;

    ssize_t bytesRead;
    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        request.write(buffer, bytesRead);

        // Check for end of HTTP header
        if (strstr(request.str().c_str(), "\r\n\r\n")) {
            break;
        }
    }

    // Output the CGI response header
    std::string response = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";

    // Execute the CGI script
    FILE* fp = popen("./cgi-bin/hello.cgi", "r");
    if (fp) {
        char c;
        while ((c = fgetc(fp)) != EOF) {
            response += c;
        }
        pclose(fp);
    }

    // Send the response to the client
    send(clientSocket, response.c_str(), response.size(), 0);

    // Close the client socket
    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t sinSize = sizeof(struct sockaddr_in);

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Set up server address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    memset(&(serverAddr.sin_zero), '\0', 8);

    // Bind socket to address
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr)) == -1) {
        std::cerr << "Error binding to address\n";
        return 1;
    }

    // Listen for connections
    if (listen(serverSocket, 10) == -1) {
        std::cerr << "Error listening for connections\n";
        return 1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (1) {
        // Accept a connection
        if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &sinSize)) == -1) {
            std::cerr << "Error accepting connection\n";
            continue;
        }

        // Handle the request in a new process
        if (fork() == 0) {
            close(serverSocket);
            handleRequest(clientSocket);
            exit(0);
        }

        // Close the client socket in the parent process
        close(clientSocket);
    }

    return 0;
}
