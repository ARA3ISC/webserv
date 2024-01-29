#include <iostream>
#include <fstream>
#include <vector>

#include <iostream>
#include <sstream>
#include <vector>
#include <cctype>
#include <climits>
std::string::iterator find_first_non_whitespace(std::string::iterator begin, std::string::iterator end) {
    while (begin != end && std::isspace(*begin)) {
        ++begin;
    }
    return begin;
}
// Function to process a chunked transfer encoding request
void processChunkedRequest(std::istringstream& iss) {
    std::vector<char> requestBody;

    // Process each chunk
    while (!iss.eof()) {
        std::string hexChunkSize;
        std::getline(iss, hexChunkSize);

        // Trim whitespace from the hexChunkSize
        std::string::iterator firstNonWhitespace = find_first_non_whitespace(hexChunkSize.begin(), hexChunkSize.end());
        hexChunkSize.erase(hexChunkSize.begin(), firstNonWhitespace);
        if (hexChunkSize.empty() || hexChunkSize == "0") {
            break; // End of chunks
        }

        std::stringstream hexStream(hexChunkSize);
        size_t chunkSize;
        hexStream >> std::hex >> chunkSize;

        std::vector<char> chunkData(chunkSize);
        iss.read(&chunkData[0], chunkSize);

        // Ignore any combination of '\r' and '\n' at the end of each chunk
        iss.ignore(INT_MAX, '\n');

        requestBody.insert(requestBody.end(), chunkData.begin(), chunkData.end());
    }

    // Process the entire body
    std::string fullBody(requestBody.begin(), requestBody.end());
    std::cout << "Received chunked data: " << fullBody << std::endl;
}

int main() {
    // Simulated received HTTP POST request with chunked transfer encoding
    std::string chunkedRequest =
        "POST /submit-data HTTP/1.1\n"
        "Host: example.com\n"
        "Transfer-Encoding: chunked\n"
        "\n"
        "5\n"
        "Hello\n"
        "6\n"
        ", World\n"
        "0\n"
        "\n";

    std::istringstream iss(chunkedRequest);

    // Skip the first line (request line)
    std::string requestLine;
    std::getline(iss, requestLine);

    // Process the headers
    std::string line;
    while (std::getline(iss, line) && !line.empty()) {
        // Process headers if needed
    }

    // Process the chunked body
    processChunkedRequest(iss);

    return 0;
}




// int main() {
//     std::ifstream inputFile("rlarabi.jpeg", std::ios::binary);

//     // Check if the input file is open
//     if (!inputFile.is_open()) {
//         std::cerr << "Error opening input file." << std::endl;
//         return 1;
//     }

//     std::ofstream outputFile("output.jpeg", std::ios::binary);

//     // Check if the output file is open
//     if (!outputFile.is_open()) {
//         std::cerr << "Error opening output file." << std::endl;
//         return 1;
//     }

//     // Read the content of the input file into a vector
//     std::vector<char> buffer(std::istreambuf_iterator<char>(inputFile), {});

//     // Write the content of the vector to the output file
//     outputFile.write(buffer.data(), buffer.size());

//     // Check if writing was successful
//     if (!outputFile) {
//         std::cerr << "Error writing to output file." << std::endl;
//         return 1;
//     }

//     std::cout << "File copied successfully." << std::endl;

//     return 0;
// }
