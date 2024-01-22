#include "../inc/dataCenter.hpp"
#include "../inc/get.hpp"

void cgi(std::string path){
    (void)path;
}

bool pathExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::string getPathWithoutSlash(std::string path){
    std::size_t last;
    for (size_t i = 0; i < path.size(); i++)
    {
        last = path.find_last_of("/");

        if (last == path.size() - 1)
            path = path.substr(0, last);
    }
    return path;
}

int getLocationRequested(std::vector<location> loc, std::string path){

    path = getPathWithoutSlash(path);

    for (size_t i = 0; i < loc.size(); i++)
        if (loc[i].getPath() == path)
            return i;

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
        // No file part, the entire path is the directory
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

bool getContentIndexedFiles(std::string path, std::vector<std::string> index,std::string &content){
    
    std::string nameFile;
    //checking the indexed file from the config file
    for (size_t i = 2; i < index.size(); i++)
    {
        nameFile = path + "/" + index[i];
        std::ifstream input(nameFile.c_str());
        if (input.is_open())
        {
            content = getContentFile(path + "/" + index[i]);
            input.close();
            return true;
        }
        input.close();
    }
    // if not checking between index.html or index.htm
    for (size_t i = 0; i < 2; i++)
    {
        nameFile = path + "/" + index[i];
        std::ifstream input(nameFile.c_str());
        if (input.is_open())
        {
            content = getContentFile(path + "/" + index[i]);
            input.close();
            return true;
        }
        input.close();
    }
    
    // std::cout << "*******directory of file to open " << path << "/" << index[0] << std::endl;
    return false;
}

void dataCenter::get(client clnt, int fd){
    std::string directory, file;

    server srv = getWebserv().getServers()[clnt.servIndx()];
    
    //split the directory and file fron the client request
    splitPath(clnt.getStartLine().path, directory, file); 
    
    // std::cout << clnt.getStartLine().path << " | " << directory << " | " << file << std::endl;

    //get the index of the location 
    int j = getLocationRequested(srv.getLocations(), directory);
    if (j == -1)
    {
        Error404(srv.get_error_pages()[404], fd);
        return ;
    }

    //the complite path of the directory or the file 
    std::string path = srv.getLocations()[j].getRoot() + clnt.getStartLine().path;
    
    //cheking existing of the file on the server
    if (!pathExists(path))
    {
        // std::cout << j << " ERROR : " << path << std::endl;
        Error404(srv.get_error_pages()[404], fd);
        return ;
    }

    // file or directory requested
    if (file != "")
    {
        std::cout << "file\n";
        cgi(path);
        return;
    }
    else
    {
        std::cout << "directory\n";

        //checking auto index
        if (srv.getLocations()[j].isAutoIndex()){
            std::string content;
            // get the files indexed and put the content in variable content 
            if (getContentIndexedFiles(path, srv.getLocations()[j].getIndexes(), content))
            {
                // sould be replaced with CGI
//                throw returnError(srv, fd, 501);

                sendResponse(fd, 200, "OK", content, "text/html");
                return ;
            } 
            else if (!srv.getLocations()[j].get_dir_listing()){ // checking if auto_index false and dir_listing false
//                sendResponse(fd, 403, "Forbidden", getContentFile("Errors/403.html"), "text/html");
//                return ;
                throw returnError(srv, fd, 403);

            }
        }

        //cheking dir listing 
        if (!srv.getLocations()[j].get_dir_listing())
        {
            throw returnError(srv, fd, 403);
//            sendResponse(fd, 403, "Forbidden", getContentFile("Errors/403.html"), "text/html");
//            return ;
        }
        else // if auto index is false or none of the indexed file exist but dir_listing is true
            listDirectory(clnt.getStartLine().path, path, fd);
    }
    std::cout << clnt.getFullRequest() << std::endl;
    // std::cout << clnt.servIndx() << "--" << path << " | " << clnt.getStartLine().path << std::endl;
}