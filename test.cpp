#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <limits.h>  // for PATH_MAX
#include <unistd.h>  // for getcwd
#include <stdio.h>
#include <cstdlib>
const int PORT = 8080;
const int BUFFER_SIZE = 1024;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void executeCgi(int clientSocket, const char *cgiScript) {
    char currentPath[PATH_MAX];
    getcwd(currentPath, sizeof(currentPath));

    char fullPath[PATH_MAX * 2];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", currentPath, cgiScript);

    char* const argv[] = { (char*)cgiScript, NULL };
    char* const envp[] = { NULL };

    dup2(clientSocket, STDOUT_FILENO);
    close(clientSocket);
    if (access(fullPath, F_OK) != -1)
    {
        std::cout << "ACEPTED\n";
    }
    else
    {
        std::cout << "notFount\n";
    }
    std::cout << fullPath << " " << argv[0] << std::endl;
     if (execve(fullPath, argv, envp) == -1) {
        // If execve fails
        std::cerr << "Error executing CGI script: " << strerror(errno) << std::endl;
        exit(1);
    }
}



void handleRequest(int clientSocket) {
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);

    // Read the client request
    int bytesRead = read(clientSocket, buffer, BUFFER_SIZE - 1);
    if (bytesRead < 0)
        error("Error reading from socket");

    // Extract the requested file (assuming it's in the format "GET /file HTTP/1.1")
    char *token = strtok(buffer, " ");
    token = strtok(NULL, " ");
    char *requestedFile = token + 1; // Skip the leading '/'

    // Prepare the HTTP response header
    const char *responseHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    write(clientSocket, responseHeader, strlen(responseHeader));

    // Execute the CGI script
    executeCgi(clientSocket, requestedFile);
}

int main() {
    int serverSocket, clientSocket;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
        error("Error opening socket");

    // Initialize server address struct
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Error on binding");

    // Listen for incoming connections
    listen(serverSocket, 5);
    std::cout << "Server listening on port " << PORT << "...\n";

    // Accept connections and handle requests
    while (1) {
        clilen = sizeof(cli_addr);
        clientSocket = accept(serverSocket, (struct sockaddr *)&cli_addr, &clilen);
        if (clientSocket < 0)
            error("Error on accept");

        // Handle the request in a separate function
        handleRequest(clientSocket);

        // Close the client socket
        close(clientSocket);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
