#include "../inc/dataCenter.hpp"
std::string getFileName(std::string root, std::string pathUpload, std::string directory, std::string extention){
    std::stringstream a;
    a << root;
    a << directory ;
    a << pathUpload;
    a << "/output";
    a << time(0);
    a << ".";
    a << extention;
    return a.str();
}


int hexToDecimal(const std::string& hexString) {
    std::istringstream iss(hexString);
    int decimalValue;
    iss >> std::hex >> decimalValue;
    if (hexString == "0")
        return 0;
    return decimalValue;
}

int getSizeChunck(std::string &buffer){
    int i = 0;
    std::string hexa;
    while((size_t)i < buffer.size() && buffer[i] != '\r'){
        hexa += buffer[i];
        i++;
    }

    if ((size_t)(i + 2) <= buffer.size())
        buffer = buffer.substr(i + 2);
    
    return hexToDecimal(hexa);
}

std::string readBufferChunck(client &clnt, std::string buffer){

    size_t i = 0;

    buffer = clnt.getTempBuffer() + buffer ;

    std::stringstream res;
    std::stringstream iss;

    if (!clnt.getbufferLen()){
        
        int s = getSizeChunck(buffer);
        clnt.setbufferLen(s);
        clnt.setChunkSize(s);
        
        if (clnt.getbufferLen() <= 0){
            clnt.setChunk(clnt.getChunk() + buffer);
            return buffer;
        }

    }

    int n = clnt.getbufferLen();

    while(i < buffer.size() && n){

        res << buffer[i];
        i++;
        n--;
    }

    clnt.setbufferLen(n);


    if (!clnt.getbufferLen()){

        if (buffer[i] == 13 && buffer[i + 1] == 10){
            i += 2;
        }


        while(i < buffer.size()){

            iss << buffer[i];
            i++;
        }


        clnt.setTempBuffer(iss.str());

        
        clnt.setChunk(clnt.getChunk() + res.str());
        return res.str();
    }
    else{// eof buffer 1024

        clnt.setTempBuffer("");
        clnt.setChunk(clnt.getChunk() + res.str());
        return res.str();
    }

    clnt.setChunk(clnt.getChunk() + res.str());
    return res.str();
}
void dataCenter::post(client &clnt, int fd){
    (void)fd;
    (void)clnt;
    std::string directory, file, res;
    if (clnt.getHeaders()["Transfer-Encoding"] == "chunked"){

        res = readBufferChunck(clnt, clnt.getbufferBody());
        // clnt.setChunk(clnt.getChunk() + res);
        // clnt.setbufferBody(res);
        if (clnt.getChunk().size() >= clnt.getChunkSize()){
            clnt.setbufferBody(clnt.getChunk());
            clnt.setChunk("");
            clnt.setChunkSize(0);
            // std::cout << "**************************\n";
        }else{
            return;
        }
        // std::cout << clnt.getChunk().size() << " " << clnt.getChunkSize() << std::endl; 
        // if (clnt.getChunk().size() == clnt.getChunkSize()){
        //     std::cout << "chunk\n"; 
        // }
    }

    if (!clnt.getIsUploadfileOpen()){
        splitPath(clnt.getStartLine().path, directory, file); 
        server srv = getWebserv().getServers()[clnt.servIndx()];    

        int j = getLocationRequested(srv.getLocations(), clnt, directory);

        std::size_t lastExtention = clnt.getHeaders()["Content-Type"].find_first_of("/");

        std::string fileName  = getFileName(srv.getLocations()[j].getRoot(), srv.getLocations()[j].getUpload(), directory, clnt.getHeaders()["Content-Type"].substr(lastExtention + 1));

        
        clnt.openFileUpload(fileName);
        
        clnt.setIsUploadfileOpen(true);
        if (!clnt.getFileUpload().is_open()) {
            std::cout << "error opening opload file\n";
            throw clnt.getResponse().setAttributes(500, "html");
        }
    }
    if (!clnt.getbufferBody().empty()){
        // std::cout << "writing : " << clnt.getbufferBody().size() << std::endl;
        clnt.getFileUpload().write(clnt.getbufferBody().c_str() , clnt.getbufferBody().size());
    }
    
    if (clnt.getBody().size() >= (std::size_t)std::atoi(clnt.getHeaders()["Content-Length"].c_str())){
        
        std::cout << clnt.getBody().size() << "pppppppppp" << clnt.getHeaders()["Content-Length"] << std::endl;
        clnt.getFileUpload().close();

        throw clnt.getResponse().setAttributes(201, "html");
    }


// output1706912528.mp4
}
        