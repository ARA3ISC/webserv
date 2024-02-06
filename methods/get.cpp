#include "../inc/dataCenter.hpp"
#include "../inc/get.hpp"

// void cgi(std::string path){
//     (void)path;
// }

bool dataCenter::pathExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::string dataCenter::cleanPath(std::string path) {
    std::string result;

    // Remove leading slashes
    size_t startPos = path.find_first_not_of('/');
    if (startPos != std::string::npos) {
        result = path.substr(startPos);
    }
    
    // Remove last slashes
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

std::string dataCenter::getCleanPath(std::string path){
    std::size_t last = path.find_last_of("?");
    if (last != std::string::npos)
        return path.substr(0, last);
    return path;
}

int dataCenter::getLocationRequested(std::vector<location> loc, client &clnt, std::string path){

    for (size_t i = 0; i < loc.size(); i++)
        if (loc[i].getPath() == path)
            return i;
    throw clnt.getResponse().setAttributes(404, "html");
}

void dataCenter::splitPath(std::string fullPath, std::string& directory, std::string& file) {

    fullPath = cleanPath(fullPath);

    std::size_t last = fullPath.find_last_of("/");

    if (last != std::string::npos){

        if(fullPath.substr(last + 1).find(".") != std::string::npos){
            directory = fullPath.substr(0, last);
            if (directory.empty())
                directory = "/";
            file = fullPath.substr(last + 1);
        }
        else// /home/iindex.html?key1=valu1
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

bool getContentIndexedFiles(std::string path, std::vector<std::string> index,std::string &content){
    
    std::string nameFile;
    //checking the indexed file from the config file
    for (size_t i = 2; i < index.size(); i++)
    {
        nameFile = path + "/" + index[i];
        std::ifstream input(nameFile.c_str());
        if (input.is_open())
        {
            content = index[i];
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
            content = index[i];
            input.close();
            return true;
        }
        input.close();
    }
    
    return false;
}

bool dataCenter::isMethodAllowed(std::vector<std::string> methods, std::string method){
    std::vector<std::string>::iterator it = std::find(methods.begin(), methods.end(), method);
    if (it != methods.end())
        return false;
    return true;
}

void getQueryStringFromPath(client &clnt, server srv, int locationIndex){
    std::string tmp = srv.getLocations()[locationIndex].getRoot() + clnt.getStartLine().path;
    
    std::size_t queryIndex = tmp.find_last_of("?");
    
    if (queryIndex != std::string::npos)
        clnt.setQueryString(tmp.substr(queryIndex + 1));
    else
        clnt.setQueryString("");
}

void dataCenter::get(client &clnt, int fd){
    std::string directory, file;
    
    server srv = getWebserv().getServers()[clnt.servIndx()];    

    //split the directory and file fron the client request
    splitPath(clnt.getStartLine().path, directory, file); 
    
    //get the index of the location 
    int j = getLocationRequested(srv.getLocations(), clnt, directory);
    std::cout <<  "location " << j << '\n';

    // seting the querystring from the complite path of the request 
    getQueryStringFromPath(clnt, srv, j);

    //the complite path of the directory or the file 
    std::string path = getCleanPath(srv.getLocations()[j].getRoot() + clnt.getStartLine().path);

    // file or directory requested
    if (!file.empty() && file.find('.') != std::string::npos)
    {
        std::cout << "file " <<   "\n";
        cgi(clnt, srv.getLocations()[j], path, 0, "");
        throw 0;
    }
    else
    {
        std::cout << "directory " << directory << "\n";

        //checking auto index
        if (srv.getLocations()[j].isAutoIndex()){
            std::string fileIndexed;
            // get the files indexed and put the content in variable content 
            if (getContentIndexedFiles(path, srv.getLocations()[j].getIndexes(), fileIndexed)){
                // it should redirect to new request with the previes request joined with the file indexed 
                // std::cout << "index : " << directory << "/" << fileIndexed << std::endl;
                // clnt.getResponse().setPath(directory + "/" + fileIndexed);
                // throw clnt.getResponse().setAttributes(301, "html");
                std::cout << "file to cgi " << srv.getLocations()[j].getRoot() << directory << "/" << fileIndexed << std::endl;
                cgi(clnt, srv.getLocations()[j], srv.getLocations()[j].getRoot() + directory + "/" + fileIndexed, 0, "");
            }
            else if (!srv.getLocations()[j].get_dir_listing()) // checking if auto_index false and dir_listing false
                throw clnt.getResponse().setAttributes(403, "html");
        }

        //cheking dir listing 
        if (!srv.getLocations()[j].get_dir_listing())
            throw clnt.getResponse().setAttributes(403, "html");
        else // if auto index is false or none of the indexed file exist but dir_listing is true
            listDirectory(srv.getLocations()[j].getRoot() + directory , directory, fd);
    }
}