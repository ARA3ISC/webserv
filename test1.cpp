#include <iostream>
#include <string>
#include <cstring>

void parseBuffer(const std::string& buffer) {
    const char* ptr = buffer.c_str();
    int bufferSize = buffer.size();

    while (true) {
        // Read chunk size
        int chunkSize;
        if (sscanf(ptr, "%x", &chunkSize) != 1) {
            // Invalid format or end of buffer
            break;
        }

        // Move the pointer to the start of the chunk data
        ptr = strstr(ptr, "\r\n") + 2;

        // Check for the end of the chunks
        if (chunkSize == 0 || ptr >= buffer.c_str() + bufferSize) {
            break;
        }

        // Extract chunk data
        std::string chunkData(ptr, chunkSize);

        // Move the pointer to the next chunk
        ptr += chunkSize;

        // Skip incomplete or missing CRLF sequence
        if (strncmp(ptr, "\r\n", 2) == 0) {
            ptr += 2; // Move past "\r\n"
        }

        // Process the chunk (you can do something with chunkSize and chunkData here)
        std::cout << "Chunk Size: " << chunkSize << ", Chunk Data: " << chunkData << std::endl;
    }
}

int main() {
    // Examples
    std::string buffer1 = "4\r\ndata\r\n7\r\nchun";
    std::string buffer2 = "4\r\ndata\r\n7\r\nchunks!\r\n0\r\n";
    std::string buffer3 = "4\r\ndata";

    // Parse the buffers
    parseBuffer(buffer1);
    parseBuffer(buffer2);
    parseBuffer(buffer3);

    return 0;
}

// #include <iostream>
// #include <cstring>

// void parseBuffer(const char* buffer, int bufferSize) {
//     const char* ptr = buffer;

//     while (true) {
//         // Read chunk size
//         int chunkSize;
//         sscanf(ptr, "%x", &chunkSize);

//         // Move the pointer to the start of the chunk data
//         ptr = strstr(ptr, "\r\n") + 2;

//         // Check for the end of the chunks
//         if (chunkSize == 0)
//             break;

//         // Extract chunk data
//         std::string chunkData(ptr, chunkSize);

//         // Move the pointer to the next chunk
//         ptr += chunkSize + 2; // +2 for "\r\n"

//         // Process the chunk (you can do something with chunkSize and chunkData here)
//         std::cout << "Chunk Size: " << chunkSize << ", Chunk Data: " << chunkData << std::endl;
//     }
// }

// int main() {
//     // Example buffer
//     const char* buffer = "4\r\ndata\r\n7\r\nchu";

//     // Size of the buffer
//     int bufferSize = std::strlen(buffer);

//     // Parse the buffer
//     parseBuffer(buffer, bufferSize);

//     return 0;
// }

// #include <iostream>
// #include <sstream>
// #include <string>
// #include <cstdlib>

// void parseChunkedBody(const std::string& chunkedData) {
//     std::istringstream iss(chunkedData);

//     while (!iss.eof()) {
//         std::string hexSizeStr;
//         std::getline(iss, hexSizeStr, '\r'); // Read until '\r' (carriage return)

//         // Ignore the '\n' (line feed) after '\r'
//         iss.ignore(1);

//         // Convert hexSizeStr to an integer
//         std::size_t chunkSize = std::strtoul(hexSizeStr.c_str(), NULL, 16);

//         if (chunkSize == 0) {
//             // End of chunks
//             break;
//         }

//         // Read the chunk data
//         std::string chunkData(chunkSize, '\0');
//         iss.read(&chunkData[0], chunkSize);

//         // Ignore the '\r\n' after the chunk data
//         iss.ignore(2);

//         // Process the chunk data (replace this with your logic)
//         std::cout << "Chunk Size: " << chunkSize << ", Chunk Data: " << chunkData << std::endl;
//     }
// }

// int main() {
//     std::string chunkedData = "4\r\n"
//     "Wiki\r\n"
//     "5\r\n"
//     "pedia\r\n"
//     "E\r\n"
//     " in\r\n"
//     "\r\n"
//     "chunks.\r\n"
//     "0\r\n"
//     "\r\n";

//     parseChunkedBody(chunkedData);

//     return 0;
// }
