#include "../inc/dataCenter.hpp"
std::string getFileName(std::string root, std::string pathUpload, std::string directory, std::string extention){
    std::stringstream a;
    a << root;
    a << directory ;
    a << pathUpload;
    a << "/output";
    a << time(0);
    a << ".";
    // (void)extention;
    a << extention;
    return a.str();
}


int hexToDecimal(const std::string& hexString) {
    std::istringstream iss(hexString);
    int decimalValue;
    iss >> std::hex >> decimalValue;
    return decimalValue;
}

void parseBuffer(const std::string& buffer) {
    std::istringstream stream(buffer);
    std::string line;

    while (std::getline(stream, line)) {
        // Read chunk size
        int chunkSize;
        std::istringstream(line) >> std::hex >> chunkSize;

        // Move to the next line (chunk data)
        std::getline(stream, line);

        // Check for the end of the chunks
        if (chunkSize == 0) {
            break;
        }

        // Extract chunk data
        std::string chunkData(line, 0, chunkSize);

        // Process the chunk (you can do something with chunkSize and chunkData here)
        std::cout << "Chunk Size: " << chunkSize << ", Chunk Data: " << chunkData << std::endl;
    }
}
// 5s\r\n
// content
// content
// content
// content
// content
// content\r\n
// 9\r\n
// lplplplplp\r\n
// 0\r\n
int getSizeChunck(std::string &buffer){
    int i = 0;
    std::string hexa;
    while((size_t)i < buffer.size() && buffer[i] != '\r'){
        hexa += buffer[i];
        i++;
    }
    std::cout << "size of hex " << i << " {" << hexToDecimal(hexa) << "}\n";
    if ((size_t)(i + 2) <= buffer.size())
        buffer = buffer.substr(i + 2);
    return hexToDecimal(hexa);
}

std::string readBufferChunck(client &clnt, std::string buffer){
    std::string tmp;
    std::stringstream iss;
    iss << buffer;
    size_t i = 0;


    std::stringstream res;
    if (!clnt.getbufferLen()){
        // std::cout << "geting to read the new size chunk\n";
        clnt.setbufferLen(getSizeChunck(buffer));    
        
        if (clnt.getbufferLen() <= 0){
            return "";
        }
        // std::cout << "size : "<< clnt.getbufferLen() << std::endl;
    }

    while(i < buffer.size() && clnt.getbufferLen()){
        res << buffer[i];
        i++;
        clnt.setbufferLen(clnt.getbufferLen() - 1);
    }
    if (i == buffer.size()){
        // std::cout << "i == size buffer\n";
        clnt.setbufferLen(clnt.getbufferLen() - i);
        if (!clnt.getTempBuffer().empty()){
            std::string tmp = clnt.getTempBuffer() + res.str();
            clnt.setTempBuffer("");
            return tmp;
        }
        return res.str();
    }
    if (buffer[i] == '\r')
    {
        // std::cout << " bufferlen to r \n";
        clnt.setbufferLen(0);
        if (res.str().size()){
            clnt.setTempBuffer(buffer.substr(0, i));
        }
        readBufferChunck(clnt, buffer.substr(i + 2));
    }
    
    return res.str();
}

void dataCenter::post(client &clnt, int fd){
    (void)fd;
    (void)clnt;
    
    std::string directory, file, res;
    
    if (clnt.getHeaders()["Transfer-Encoding"] == "chunked"){
        // std::cout << "body : " << clnt.getbufferBody() << std::endl;
        // static int size = 0;
        res = readBufferChunck(clnt,clnt.getbufferBody());
        clnt.setbufferBody(res);
        std::cout << "|" << res.size() << "|\n";
        // std::cout << res.size() << " buffer read\n";
        //     std::cout << "m\n";  \n
        // std::cout << "size after : " << size << std::endl;
        // return ;
    }










    if (!clnt.getIsUploadfileOpen()){
        splitPath(clnt.getStartLine().path, directory, file); 
        server srv = getWebserv().getServers()[clnt.servIndx()];    

        int j = getLocationRequested(srv.getLocations(), clnt, directory);

        std::size_t lastExtention = clnt.getHeaders()["Content-Type"].find_first_of("/");

        std::string fileName  = getFileName(srv.getLocations()[j].getRoot(), srv.getLocations()[j].getUpload(), directory, clnt.getHeaders()["Content-Type"].substr(lastExtention + 1));

        
        clnt.openFileUpload(fileName);
        // std::cout << "file : " << a.str() << " " << clnt.getFileUpload().is_open() << std::endl;
        clnt.setIsUploadfileOpen(true);
        if (!clnt.getFileUpload().is_open()) {
            std::cout << "error opening opload file\n";
            throw clnt.getResponse().setAttributes(500, "text/html");
        }
    }
// 4,451,143

    if (clnt.getBody().size() <= (std::size_t)std::atoi(clnt.getHeaders()["Content-Length"].c_str())){

        clnt.getFileUpload().write(clnt.getbufferBody().c_str() , clnt.getbufferBody().size());

    }
    
    if (clnt.getBody().size() >= (std::size_t)std::atoi(clnt.getHeaders()["Content-Length"].c_str())){
        
        std::cout << clnt.getBody().size() << "pppppppppp\n" << clnt.getHeaders()["Content-Length"].size();
        clnt.getFileUpload().close();

        throw clnt.getResponse().setAttributes(201, "text/html");
    }


    // if (clnt.getBody().size() >= static_cast<std::size_t>(std::atoi(clnt.getHeaders()["Content-Length"].c_str()))){
    // }
    // if transfer-encoder == chuncked

    // if transfer-encoder not found !

}