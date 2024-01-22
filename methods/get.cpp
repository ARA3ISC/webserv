#include "../inc/dataCenter.hpp"
#include "../inc/get.hpp"

void cgi(std::string path){
    (void)path;
}

bool pathExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

int getLocationRequested(std::vector<location> loc, std::string path){
    // std::vector<location>::iterator it = loc.begin();
    std::size_t last = path.find_last_of("/");
    if (last == path.size() - 1)
       path = path.substr(0, last);

    for (size_t i = 0; i < loc.size(); i++)
    {
        std::cout << loc[i].getPath() << " */* " << path << std::endl;
        if (loc[i].getPath() == path)
            return i;
    }
    return -1;
}

void splitPath(const std::string& fullPath, std::string& directory, std::string& file) {
    // Find the last '/'
    std::size_t lastSeparator = fullPath.find_last_of("/");
    
    if (lastSeparator == 0)
    {
        fullPath.substr(0);
        lastSeparator = fullPath.find_last_not_of("/");
    }

    if (lastSeparator != std::string::npos) {
        // Extract the directory and file parts
        directory = fullPath.substr(0, lastSeparator + 1);
        file = fullPath.substr(lastSeparator + 1);
    } else {
        // No directory part, the entire path is the file
        directory = fullPath;
        file = "";
    }
}

void listDirectory(std::string path, std::string directory, int fd){
    
    DIR* dir;
    struct dirent* ent;
    std::string response = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Directory Listing</title></head><body><h1>Directory Listing</h1><ul>";
    if ((dir = opendir(directory.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string tmp = ent->d_name;
            if (tmp == ".." || tmp == ".")
                continue;
            response += "<li><a href=\""  + path + "/" + tmp + "\" >" + tmp + "</a></li>";
        }
        closedir(dir);
    }
    response += "</ul></body></html>";
    sendResponse(fd, 200, "OK", response, "text/html");
}

void dataCenter::get(client clnt, int fd){
    std::string directory, file;

    server srv = getWebserv().getServers()[clnt.servIndx()];
    
    splitPath(clnt.getStartLine().path, directory, file); 
    
    std::cout << clnt.getStartLine().path << " | " << directory << " | " << file << std::endl;

    int j = getLocationRequested(srv.getLocations(), directory);
    
    std::string path = srv.getLocations()[j].getRoot() + clnt.getStartLine().path;

    // no matching loaction or the location not found in the server
    if (j == -1 || !pathExists(path))
    {
        std::cout << j << " ERROR : " << path << std::endl;
        Error404(srv.get_error_pages()[404], fd);
        // return ;
    }
    if (directory[directory.size() - 1] == '/')
    {
        std::cout << "file\n";
        cgi(path);
    }
    else
    {
        std::cout << "directory\n";
        if (srv.getLocations()[j].isAutoIndex()){
            // sendResponse(fd, 200, "OK", getContentFile("Errors/403.html"), "text/html");

        }
        if (!srv.getLocations()[j].get_dir_listing())
        {
            sendResponse(fd, 403, "Forbidden", getContentFile("Errors/403.html"), "text/html");
            return ;
        }
            // Error404(srv.get_error_pages()[404], fd);

        if (srv.getLocations()[j].isAutoIndex())
        {
            // auto index
            
        }
        else
            listDirectory(clnt.getStartLine().path, path, fd);
    }
    std::cout << clnt.getFullRequest() << std::endl;
    std::cout << clnt.servIndx() << "--" << path << " | " << clnt.getStartLine().path << std::endl;
}