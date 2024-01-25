#include "../inc/dataCenter.hpp"
static int getLocationRequested(std::vector<location> loc, std::string path){

    for (size_t i = 0; i < loc.size(); i++)
        if (loc[i].getPath() == path)
            return i;

    return -1;
}
static bool isMethodAllowed(std::vector<std::string> methods, std::string method){
    std::vector<std::string>::iterator it = std::find(methods.begin(), methods.end(), method);
    if (it != methods.end())
        return false;
    return true;
}
static std::string getCleanPath(std::string path){
    std::size_t last = path.find_last_of("?");
    if (last != std::string::npos)
        return path.substr(0, last);
    return path;
}
static std::string cleanPath(std::string path) {
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
static void splitPath(std::string fullPath, std::string& directory, std::string& file) {

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


void dataCenter::post(client clnt, int fd){
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
    if(isMethodAllowed(srv.getLocations()[j].getMethods(), "POST"))
        throw returnError(srv, fd, 405);

    std::stringstream postData;

    std::ofstream outputFile("output.jpeg", std::ios::binary);

    if (outputFile.is_open()) {
        outputFile.write(clnt.getBody().c_str(), clnt.getBody().size());
        outputFile.close();
        std::cout << "File received and saved successfully." << std::endl;
    } else {
        std::cerr << "Error opening the output file." << std::endl;
    }
    // std::fstream input("./myWebsite/upload/myPhoto.txt");
    // if (input.is_open()){
    //     input << clnt.getBody();
    //     input.close();
    // }
    // else{
    //     std::cout << "error opening" << std::endl;
        
    // }
    std::cout << clnt.getBody().size() << "*************" << clnt.getHeaders()["Content-Length"] << std::endl;
}