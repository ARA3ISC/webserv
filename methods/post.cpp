#include "../inc/dataCenter.hpp"

std::string dataCenter::getFileName(std::string root, std::string pathUpload, std::string directory, std::string extention){
    std::stringstream a;
    a << root;
    a << directory ;
    a << pathUpload;
    a << "/output";
    a << time(0);
    a << this->getFilePrefix();
    a << ".";
    a << extention;
    return a.str();
}


int hexToDecimal(const std::string& hexString) {
    // std::cout << "hexa : " << hexString << std::endl;
    if (hexString == ""){
        std::cout << "set to zero\n";
        return 0;
    }
    std::istringstream iss(hexString);
    int decimalValue;
    iss >> std::hex >> decimalValue;
    return decimalValue;
}
 
int getSizeChunck(std::string &buffer){
    int i = 0;
    std::string hexa;
    if (buffer[i] == '\r' && buffer[i + 1] == '\n'){
        i += 2;
    }
    while((size_t)i < buffer.size() && buffer[i] != '\r'){
        hexa += buffer[i];
        i++;
    }

    if ((size_t)(i + 2) <= buffer.size())
        buffer = buffer.substr(i + 2);
    
    return hexToDecimal(hexa);
}

void readBufferChunck(client &clnt, std::string buffer){

    size_t i = 0;
    
    buffer = clnt.getTempBuffer() + buffer ;

    std::string result;
    std::string tmp;

    if (!clnt.getbufferLen()){
        
        int s = getSizeChunck(buffer);
        clnt.setbufferLen(s);
        clnt.setChunkSize(s);

    }

    int n = clnt.getbufferLen();

    if ((unsigned long)n < buffer.size()){
        result = buffer.substr(0, n);
        i = n;
        n = 0;
    }else{
        result = buffer;
        i += buffer.size();
        n -= buffer.size();
    }

    clnt.setbufferLen(n);

    if (!n){
        
        if (buffer[i] == '\r' && buffer[i + 1] == '\n'){
            i += 2;
        }

        clnt.setTempBuffer(buffer.substr(i));

        clnt.setChunk(result);
        return ;
    }
    else{// eof buffer 1024

        clnt.setTempBuffer("");
        clnt.setChunk(result);
        return ;
    }
}

void dataCenter::post(client &clnt, int fd){
    (void)fd;
    std::string directory, file, res;
    if (clnt.getHeaders()["Transfer-Encoding"] == "chunked" && !clnt.getbufferBody().empty()){

        readBufferChunck(clnt, clnt.getbufferBody());
        
        if (clnt.getChunk().size() >= clnt.getChunkSize()){
            clnt.setbufferBody(clnt.getChunk());
            clnt.setChunk("");
            clnt.setChunkSize(0);
        }else{
            return;
        }
    }
    
    if (!clnt.getIsUploadfileOpen()){
        splitPath(clnt, directory, file); 
        
        server srv = getWebserv().getServers()[clnt.servIndx()];    

        int j = clnt.getLocationIndex();

        std::size_t lastExtention = clnt.getHeaders()["Content-Type"].find_first_of("/");
        std::string extension = clnt.getHeaders()["Content-Type"].substr(lastExtention + 1);
        if (extension == "x-www-form-urlencoded")
            extension = "txt";
        std::string fileName  = getFileName(srv.getLocations()[j].getRoot(), srv.getLocations()[j].getUpload(), directory, extension);

        std::cout << fileName << std::endl;
        clnt.openFileUpload(fileName);

        clnt.setIsUploadfileOpen(true);
        if (!clnt.getFileUpload().is_open()) {
            std::cout << "error opening upload file\n";
            throw clnt.getResponse().setAttributes(500, "html");
        }

    }
    if (!clnt.getbufferBody().empty()){
        clnt.setFullSize(clnt.getbufferBody().size());
        clnt.getFileUpload().write(clnt.getbufferBody().c_str(), clnt.getbufferBody().size());

    }
    std::istringstream iss(clnt.getHeaders()["Content-Length"]);
    size_t nb;
    iss >> nb;
    
    if (clnt.getFullSize() >= nb){
        
        std::cout << clnt.getFullSize() << "<- file size | content Len ->" << clnt.getHeaders()["Content-Length"] << std::endl;
        clnt.getFileUpload().close();
        clnt.setFullSize(0);

        splitPath(clnt, directory, file); 

        // splitPath(clnt.getStartLine().path, directory, file); 
        server srv = getWebserv().getServers()[clnt.servIndx()];    
        int j = clnt.getLocationIndex();

        if (!file.empty()){
            //cgi
            std::cout << "file to cgi : " << file << std::endl;
            cgi(clnt, srv.getLocations()[j], file, 1, clnt.getFileUploadName());
        }else
            throw clnt.getResponse().setAttributes(201, "html");
    }


}
        