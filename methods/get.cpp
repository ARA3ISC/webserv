#include "../inc/dataCenter.hpp"
#include "../inc/get.hpp"

// void cgi(std::string path){
//     (void)path;
// }

bool pathExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}
bool isDirectory(const char* path) {
    struct stat fileStat;
    
    if (stat(path, &fileStat) != 0) {
        // Error handling: unable to retrieve file status
        std::cerr << "Error getting file status for " << path << std::endl;
        return false;
    }

    return S_ISDIR(fileStat.st_mode);
}
std::string cleanPath(std::string path) {
    std::string result;

    // Remove leading slashes
    size_t startPos = path.find_first_not_of('/');
    if (startPos != std::string::npos) {
        result = path.substr(startPos);
    }

    // Remove trailing slashes
    size_t endPos = result.find_last_not_of('/');
    if (endPos != std::string::npos) {
        result = result.substr(0, endPos + 1);
    }

    // Add a single leading slash
    if (!result.empty() && result[0] != '/') {
        result = '/' + result;
    }
    // add s single slash if it's empty
    if (result.empty())
        result = "/";
    return result;
}

std::string getCleanPath(std::string path){
    std::size_t last = path.find_last_of("?");
    if (last != std::string::npos)
        return path.substr(0, last);
    return path;
}

int getLocationRequested(std::vector<location> loc, std::string path){

    for (size_t i = 0; i < loc.size(); i++)
        if (loc[i].getPath() == path)
            return i;

    return -1;
}

void splitPath(std::string fullPath, std::string& directory, std::string& file) {

    fullPath = cleanPath(fullPath);
    
    std::size_t last = fullPath.find_last_of("/");

    if (last != std::string::npos){
        
        if(fullPath.substr(last + 1).find(".") != std::string::npos){
            directory = fullPath.substr(0, last);
            if (directory.empty())
                directory = "/";
            file = fullPath.substr(last + 1);
        }
        else
        {
            directory = fullPath;
            file = "";
        }
    }
    else
    {
        directory = fullPath;
        file = "";
    }
    
    // removing ? form the uri
    directory = getCleanPath(directory);
    file = getCleanPath(file);
}

void listDirectory(std::string path, std::string directory, int fd){
    
    DIR* dir;
    struct dirent* ent;
    std::string response = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Directory Listing</title></head><body><h1>Directory Listing</h1><ul>";
    if ((dir = opendir(path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string tmp = ent->d_name;
            if (tmp == ".." || tmp == ".")
                continue;
            if (directory == "/")
                response += "<li><a href=\""  + directory + tmp + "\" >" + tmp + "</a></li>";
            else
                response += "<li><a href=\""  + directory + "/" + tmp + "\" >" + tmp + "</a></li>";
        }
        closedir(dir);
    }
    response += "</ul></body></html>";
    sendResponse(fd, 200, "OK", response, "text/html");
    throw 0;
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
            content = path + "/" + index[i];
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
            content = path + "/" + index[i];
            input.close();
            return true;
        }
        input.close();
    }
    
    return false;
}

bool isMethodAllowed(std::vector<std::string> methods, std::string method){
    std::vector<std::string>::iterator it = std::find(methods.begin(), methods.end(), method);
    if (it != methods.end())
        return false;
    return true;
}

void dataCenter::get(client clnt, int fd){
    std::string directory, file;
    
    server srv = getWebserv().getServers()[clnt.servIndx()];    

    //split the directory and file fron the client request
    splitPath(clnt.getStartLine().path, directory, file); 
    
    //get the index of the location 
    int j = getLocationRequested(srv.getLocations(), directory);
    
    if (j == -1) {
        std::cout << "****\n";
        throw returnError(srv, fd, 404);
    }

    //is method allowed in config file
    if(isMethodAllowed(srv.getLocations()[j].getMethods(), "GET"))
        throw returnError(srv, fd, 405);
    
    //the complite path of the directory or the file 
    std::string path = getCleanPath(srv.getLocations()[j].getRoot() + clnt.getStartLine().path);
    
    //cheking existing of the file on the server
    
    if (!pathExists(path))
        throw returnError(srv, fd, 404);

    // file or directory requested
    if (!file.empty() && file.find('.') != std::string::npos)
    {
        std::cout << "file " <<   "\n";
        cgi(clnt.servIndx(), srv.getLocations()[j], path, fd);
    }
    else
    {
        std::cout << "directory " << directory << "\n";

        //checking auto index
        if (srv.getLocations()[j].isAutoIndex()){
            std::string fileIndexed;
            // get the files indexed and put the content in variable content 
            if (getContentIndexedFiles(path, srv.getLocations()[j].getIndexes(), fileIndexed))
                cgi(clnt.servIndx(), srv.getLocations()[j], fileIndexed, fd);
            else if (!srv.getLocations()[j].get_dir_listing()) // checking if auto_index false and dir_listing false
                throw returnError(srv, fd, 403);
        }

        //cheking dir listing 
        if (!srv.getLocations()[j].get_dir_listing())
            throw returnError(srv, fd, 403);
        else // if auto index is false or none of the indexed file exist but dir_listing is true
            listDirectory(srv.getLocations()[j].getRoot() + directory , directory, fd);
    }
    // std::cout << clnt.getFullRequest() << std::endl;
    // std::cout << clnt.servIndx() << "--" << path << " | " << clnt.getStartLine().path << std::endl;
}