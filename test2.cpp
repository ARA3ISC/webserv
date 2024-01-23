#include <iostream>
#include <unistd.h>

int main() {
    int pipefd[2]; // File descriptors for the pipe

    // Create the pipe
    if (pipe(pipefd) == -1) {
        std::cerr << "Pipe creation failed." << std::endl;
        return 1;
    }

    pid_t pid = fork(); // Fork a new process

    if (pid == -1) {
        std::cerr << "Fork failed." << std::endl;
        return 1;
    }

    if (pid == 0) {
        // Child process

        // Close the read end of the pipe in the child
        close(pipefd[0]);

        // Redirect stdout to the pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Replace the child process image with a new executable
        const char* command = "/bin/usr/sh";
        char* const argv[] = {(char*)command, (char*)"Hello from child process!", NULL};
        char* const envp[] = {NULL};
        execve(command, argv, envp);

        // execve will only return if there's an error
        std::cerr << "execve failed." << std::endl;
        return 1;
    } else {
        // Parent process

        // Close the write end of the pipe in the parent
        close(pipefd[1]);

        char buffer[100];
        ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer));
        close(pipefd[0]); // Close the read end of the pipe in the parent

        if (bytesRead == -1) {
            std::cerr << "Read from pipe failed." << std::endl;
            return 1;
        }

        std::cout << "Parent process received: " << buffer << std::endl;
    }

    return 0;
}
