#include "../inc/dataCenter.hpp"

void cgi(std::string path){
    
}

bool pathExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::string getDefaultError404(){
    std::string err;
    std::string line;
    std::string path = "Errors/404.html";
    std::ifstream input(path.c_str());

    if (input.is_open()){
        while (std::getline(input, line)) {
            err += line;
        }
    }
    return err;
}

std::string getContentFile(std::string path){
    std::string fileContent = "";
    std::string line;
    std::ifstream input(path.c_str());

    if (input.is_open()){
        while (std::getline(input, line)) {
            fileContent += line;
        }
    }
    return fileContent;
}

void sendResponse(int clientSocket, int statusCode, std::string statusMessage, std::string content, std::string contentType) {
    // Construct the HTTP response
    std::ostringstream httpResponse;
    httpResponse << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    httpResponse << "Content-Type: " << contentType << "\r\n";
    httpResponse << "Content-Length: " << content.length() << "\r\n";
    httpResponse << "\r\n";
    httpResponse << content;

    // std::cout << "httpResponse : " << httpResponse.str() << std::endl;
    // Send the response back to the client
    write(clientSocket, httpResponse.str().c_str(), httpResponse.str().length());
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

void Error404(std::string fileName, int fd){


    //content of the file provided in config file
    std::string content = getContentFile(fileName);
    // in case error file not provivde in config file we get our default file in Errors folder
    if(content == "")
        content = getDefaultError404();
    sendResponse(fd, 404, "Not Found", content, "text/html; charset=utf-8");
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