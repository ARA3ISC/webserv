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
// /home/f1/f2/file.kpk

int dataCenter::getLocationRequested(std::vector<location> loc, std::string path){

    for (size_t i = 0; i < loc.size(); i++)
        if (loc[i].getPath() == path)
            return i;
    return -1;
}

bool dataCenter::isDirectory(const std::string& path) {
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) != 0) {
        // Failed to retrieve file status
        return false;
    }

    return S_ISDIR(fileStat.st_mode);
}
void removeTrailingSlashes(std::string& str) {
    std::string::size_type pos = str.find_last_not_of('/');

    if (pos != std::string::npos) {
        str.erase(pos + 1);
    }
}

void getQueryStringFromPath(client &clnt, std::string &toOpen){
    
    size_t pos = toOpen.find_last_of("?");
    if (pos != std::string::npos){
        clnt.setQueryString(toOpen.substr(pos + 1));
        toOpen = toOpen.substr(0, pos);
    }
}

void dataCenter::splitPath(client &clnt,std::string& directory, std::string& file) {
    int index = clnt.getLocationIndex();
    server srv = getWebserv().getServers()[clnt.servIndx()];
    
    std::string path = srv.getLocations()[index].getPath();
    std::string url = clnt.getStartLine().path;

    std::string trimUrl = trimFromBeginning(url, path);
    if (trimUrl[0] != '/')
        trimUrl.insert(0, "/");
    std::string toOpen = srv.getLocations()[index].getRoot() + trimUrl;

    getQueryStringFromPath(clnt, toOpen);

    // std::cout << "to open " << isDirectory(toOpen) << " " << toOpen << std::endl;
    if (!isDirectory(toOpen)){
        removeTrailingSlashes(toOpen);
        int fd = open(toOpen.c_str(), O_RDONLY);
        if (fd == -1)
            throw clnt.getResponse().setAttributes(404, "html");
        close(fd);
        file = toOpen;
        return ; 
    }
    directory = toOpen;
}

bool getContentIndexedFiles(std::string path, std::vector<std::string> index,std::string &content){
    
    std::string nameFile;
    
    for (size_t i = 0; i < index.size(); i++)
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

bool pathHasSlashAtEnd(std::string path){
    if(path.size() != 1 && path[path.size() -1] != '/')
        return true;
    return false;
}

void dataCenter::get(client &clnt, int fd){
    std::string directory, file;
    
    int j = clnt.getLocationIndex();
    
    server srv = getWebserv().getServers()[clnt.servIndx()];    


    splitPath(clnt, directory, file); 

    // getQueryStringFromPath(clnt, file, directory);

    if (!file.empty())
    {
        // std::cout << "file " <<  file << "\n";
        cgi(clnt, srv.getLocations()[j], file, 0, "");
        throw 0;
    }
    else
    {
        if (pathHasSlashAtEnd(clnt.getStartLine().path)){
            clnt.getResponse().setPath(clnt.getStartLine().path + "/");
            throw clnt.getResponse().setAttributes(301, "html");
        }

        if (srv.getLocations()[j].isAutoIndex()){
            std::string fileIndexed;
            
            if (getContentIndexedFiles(directory, srv.getLocations()[j].getIndexes(), fileIndexed)){
                 cgi(clnt, srv.getLocations()[j], directory + "/" + fileIndexed , 0, "");
            }
            else if (!srv.getLocations()[j].get_dir_listing())
                throw clnt.getResponse().setAttributes(403, "html");
        }

        if (!srv.getLocations()[j].get_dir_listing())
            throw clnt.getResponse().setAttributes(403, "html");
        else
            listDirectory(directory , directory, fd);
    }
}
