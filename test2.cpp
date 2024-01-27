#include <iostream>
#include <fstream>

int main() {
    const char* charFileName = "example_char.txt";
    std::string stringFileName = "example_string.txt";

    // Using const char*
    std::fstream charFileStream(charFileName, std::ios::out);
    if (!charFileStream.is_open()) {
        std::cerr << "Failed to open the file: " << charFileName << std::endl;
        return 1;
    }
    charFileStream << "Hello, World!\n";
    charFileStream.close();

    // Using std::string
    std::fstream stringFileStream(stringFileName, std::ios::out);
    if (!stringFileStream.is_open()) {
        std::cerr << "Failed to open the file: " << stringFileName << std::endl;
        return 1;
    }
    stringFileStream << "This is a file with a string filename.\n";
    stringFileStream.close();

    std::cout << "Files created successfully." << std::endl;

    return 0;
}
