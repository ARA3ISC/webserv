#include <iostream>
#include <string>

std::string getPathInfo(const std::string& url) {
    std::string pathInfo;
    size_t pos = url.find("://"); // Find the end of the protocol part
    pos = url.find('/', pos + 3); // Find the first '/' after the protocol
    if (pos != std::string::npos) {
        // Find the position of the next '/' after the first '/'
        size_t nextSlashPos = url.find('/', pos + 1);
        if (nextSlashPos != std::string::npos) {
            // Find the position of the following '/' after the first '/'
            size_t followingSlashPos = url.find('/', nextSlashPos + 1);
            if (followingSlashPos != std::string::npos) {
                // Extract the path info after the first file
                pathInfo = url.substr(followingSlashPos);
            }
        }
    }
    return pathInfo;
}

int main() {
    std::string url = "/POST/dfffff/uploadffFile.pfffhp/addfffi/pathffinfo";
    std::string pathInfo = getPathInfo(url);
    std::cout << "Path info: " << pathInfo << std::endl;
    return 0;
}
