#include <iostream>
#include <string>

std::string getPathInfo(const std::string& url) {
    std::string pathInfo;
    size_t pos = url.find("://");
    pos = url.find('/', pos + 3);
    if (pos != std::string::npos) {
        size_t nextSlashPos = url.find('/', pos + 1);
        if (nextSlashPos != std::string::npos) {
            size_t followingSlashPos = url.find('/', nextSlashPos + 1);
            if (followingSlashPos != std::string::npos) {
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
