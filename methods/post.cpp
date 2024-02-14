#include "../inc/dataCenter.hpp"

std::string dataCenter::getFileName(std::string pathUpload, std::string directory, std::string extention){
    std::stringstream a;

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
    if (hexString == ""){
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
    else{

        clnt.setTempBuffer("");
        clnt.setChunk(result);
        return ;
    }
}

void dataCenter::post(client &clnt){
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

        if (srv.getLocations()[j].getUpload().empty())
            throw clnt.getResponse().setAttributes(403, "html");

        std::size_t lastExtention = clnt.getHeaders()["Content-Type"].find_first_of("/");
        std::string extension = clnt.getHeaders()["Content-Type"].substr(lastExtention + 1);
        if (extension == "x-www-form-urlencoded")
            extension = "txt";
        std::string fileName;
        fileName = getFileName(srv.getLocations()[j].getUpload(), srv.getLocations()[j].getRoot(), extension);

        clnt.openFileUpload(fileName);

        clnt.setIsUploadfileOpen(true);


        if (!clnt.getFileUpload().is_open())
            throw clnt.getResponse().setAttributes(500, "html");

    }
    if (!clnt.getbufferBody().empty()){
        clnt.setFullSize(clnt.getbufferBody().size());
        clnt.getFileUpload().write(clnt.getbufferBody().c_str(), clnt.getbufferBody().size());

    }
    std::istringstream iss(clnt.getHeaders()["Content-Length"]);
    size_t nb;
    iss >> nb;

    if (clnt.getFullSize() >= nb){

        clnt.getFileUpload().close();
        clnt.setFullSize(0);

        splitPath(clnt, directory, file);

        server srv = getWebserv().getServers()[clnt.servIndx()];
        int j = clnt.getLocationIndex();

        if (!file.empty()){
            cgi(clnt, srv.getLocations()[j], file, 1, clnt.getFileUploadName());
        }else{
            if (srv.getLocations()[j].isAutoIndex()){
                std::string fileIndexed;

                if (getContentIndexedFiles(directory, srv.getLocations()[j].getIndexes(), fileIndexed)){
                
                    if (!checkCgiPaths(srv.getLocations()[j], directory + fileIndexed))
                        cgi(clnt, srv.getLocations()[j], directory + fileIndexed ,1, clnt.getFileUploadName());
                
                }
                else
                    throw clnt.getResponse().setAttributes(201, "html");

            }
            else
                throw clnt.getResponse().setAttributes(201, "html");
        }
    }


}
