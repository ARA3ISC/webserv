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
    if (hexString == "0")
        return 0;
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
    std::cout << "size of hex " << i << " {" << hexToDecimal(hexa) << "} "<< (int)buffer[i] << " " << (int)buffer[i + 1] << " " << (int)buffer[i + 2]<< " \n";
    // if (i == 0)
    //     return 0;

    if ((size_t)(i + 2) <= buffer.size())
        buffer = buffer.substr(i + 2);
    else
        std::cout << "err\n";
    
    return hexToDecimal(hexa);
}


bool isLastChunk(std::string chunk) {
    // std::istringstream stream(chunk);
    // int chunkSize;
    // stream >> std::hex >> chunkSize;

    // // Check if the chunk size is 0
    // return chunkSize == 0;
    return chunk.find("0\r\n\r\n") != std::string::npos;
}

bool a = false;
int ll = 0;
std::string readBufferChunck(client &clnt, std::string buffer){

    size_t i = 0;
    size_t j = 0;
    if (a){
        while(j < buffer.size() && buffer[j] != 13)
            j++;
        std::cout << j << " :j " << (int)buffer[j + 1] << " "  << (int)buffer[j + 2] << " \n";
    }
    buffer = clnt.getTempBuffer() + buffer ;

    std::stringstream res;
    std::stringstream iss;

    if (!clnt.getbufferLen()){
        
        std::cout << "geting to read the new size chunk\n";

        clnt.setbufferLen(getSizeChunck(buffer));    
        
        if (clnt.getbufferLen() <= 0){
            std::cout << "empty\n";
            return buffer;
        }
        std::cout << "size : "<< clnt.getbufferLen() << std::endl;
    }

    int n = clnt.getbufferLen();

    while(i < buffer.size() && n){

        res << buffer[i];
        i++;
        n--;
    }

    clnt.setbufferLen(n);
    

    std::cout << "n : " << n <<"\n";

    if (!clnt.getbufferLen()){

        if (buffer[i] != 13 && buffer[i + 1] != 10){
            clnt.setbufferBody("");
            return res.str();
        }
        if (buffer[i] == 13 && buffer[i + 1] == 10){
            std::cout << "ignore 2\n";
            i += 2;
        }

        std::cout << "{ ";
        while(i < buffer.size()){
            std::cout << (int)buffer[i] << " "; 
            iss << buffer[i];
            i++;
        }

        std::cout << " }\n";

        clnt.setTempBuffer(iss.str());
        std::cout << "sizeof temp : " << clnt.getTempBuffer().size() << std::endl; 
        

        return res.str();
    }
    else{// eof buffer 1024
        std::cout << "eof buffer\n";
        clnt.setbufferBody("");

        return res.str();
    }


    // std::cout << "res : " << buffer.size() << " ---> " << isLastChunk(buffer) << std::endl;
    // std::cout << buffer.size() << "--" << (buffer.find("0\r\n\r\n") != std::string::npos) << std::endl;

    std::cout << "return \n";
    return res.str();
}
// 4\r\n
// abcd\r\n
// 0\r\n
// \r\n
void dataCenter::post(client &clnt, int fd){
    (void)fd;
    (void)clnt;
    
    std::string directory, file, res;
    
    if (clnt.getHeaders()["Transfer-Encoding"] == "chunked"){

        res = readBufferChunck(clnt,clnt.getbufferBody());

        clnt.setbufferBody(res);
        std::cout << "******************body : " << clnt.getbufferBody().size() << std::endl;
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
    if (!clnt.getbufferBody().empty()){
        // std::cout << "|" << clnt.getbufferBody() << "|"<< std::endl;
        std::cout << "writing \n";
        ll += clnt.getbufferBody().size();
        clnt.getFileUpload().write(clnt.getbufferBody().c_str() , clnt.getbufferBody().size());

    }
    
    if (clnt.getBody().size() >= (std::size_t)std::atoi(clnt.getHeaders()["Content-Length"].c_str())){
        std::cout << "ll : " << ll << std::endl;
        std::cout << clnt.getBody().size() << "pppppppppp" << clnt.getHeaders()["Content-Length"] << std::endl;
        clnt.getFileUpload().close();

        throw clnt.getResponse().setAttributes(201, "text/html");
    }



}
        

        // while(i < buffer.size()){

        //     if(buffer[i] == '\r' && i + 1 < buffer.size() && buffer[i + 1] == '\n' && (int)(buffer[i + 2]) >= 48 && (int)(buffer[i + 3]) >= 48)
        //     {
        //         break;
        //     }
        //     res << buffer[i];
        //     i++;
        // }

        // if (buffer.size() == i){   
        //     std::cout << "+++++++++++++++++++\n";
        //     clnt.setTempBuffer("");
        //     a = true;
        //     return res.str();
        // }

        // if (buffer[i] == '\r' && buffer[i + 1] == '\n'){
        //     // std::cout << "-----\n";
        //     i += 2;
        // }
        
        // std::cout << "ll : " << (int)buffer[i] << " " << (int)buffer[i + 1] << std::endl;
        // size_t j = 0;
        // while(j < clnt.getbufferBody().size()){
        //     // if ((int)clnt.getbufferBody()[j] == 13 && (int)clnt.getbufferBody()[j + 1] == 10  && (int)clnt.getbufferBody()[j + 2] == 49)

        //     std::cout << (int)clnt.getbufferBody()[j] << " "; 
        //     j++;
        // }
        // std::cout << std::endl;


        // std::cout << "|" << clnt.getbufferBody().size() << "|**********\n";
        // std::cout << res.size() << " buffer read\n";
        //     std::cout << "m\n";  \n
        // std::cout << "size after : " << size << std::endl;
        // return ;
// 4,451,143

    // if (clnt.getBody().size() <= (std::size_t)std::atoi(clnt.getHeaders()["Content-Length"].c_str())){
    //     clnt.getFileUpload().write(clnt.getbufferBody().c_str() , clnt.getbufferBody().size());

    // }

    // if (clnt.getBody().size() >= static_cast<std::size_t>(std::atoi(clnt.getHeaders()["Content-Length"].c_str()))){
    // }
    // if transfer-encoder == chuncked

    // if transfer-encoder not found !




    
    // size_t j = -1;

    // while(++j < buffer.size())
    //     std::cout << (int)buffer[j] << " " ;
    

    // if (!clnt.getbufferLen()){
    //     std::cout << "read all the chunk\n";
    // }





    // if (res.str() == "\r\n"){
    //     res.ignore(2);
    //     i += 2;
    // }

    
    // std::cout << "buffer : " << buffer.size() << std::endl;












    // if (isLastChunk(buffer)){
    //     std::cout << "the last chunk\n";
    // }

    // if (!buffer.size()){
    //     std::cout << "endof chunk\n";
    // }
    // else{
    //     std::cout << buffer.size() << "--" << (buffer.find("\r\n") != std::string::npos) << std::endl;
    // }

    // if (i == buffer.size()){
    //     std::cout << "endof buffer\n";
    // }

    // std::cout << i << " size of i and |" << res.str().size() << "| --->  "<< isLastChunk(buffer) <<"\n";
    
    
    
    
    
    
    
    // if (i == buffer.size()){
    //     // clnt.setbufferLen(clnt.getbufferLen() - i);
    //     std::cout << "i == size buffer and "<< clnt.getbufferLen()<<"\n";
    //     if (!clnt.getTempBuffer().empty()){
    //         std::string tmp = clnt.getTempBuffer() + res.str();
    //         clnt.setTempBuffer("");
    //         std::cout << "join buffer\n";
    //         return tmp;
    //     }
    //     return res.str();
    // }
    // if (clnt.getbufferLen() <= 0)
    // {
    //     std::cout << "end of bufferlen to  \\r \n";
    //     clnt.setbufferLen(0);
    //     if (res.str().size()){
    //         clnt.setTempBuffer(buffer.substr(0, i));
    //         std::cout << "seting temp buffer |"<< clnt.getTempBuffer().size()<<"|\n";
    //     }
    //     if (o)
    //         std::cout << "last chunk\n";
    //     buffer = buffer.substr(i + 2);
    //     readBufferChunck(clnt, buffer);
    // }