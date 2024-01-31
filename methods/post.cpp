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
// 5\r\n
// content\r\n
// 9\r\n
// lplplplplp\r\n
// 0\r\n
int hexToDecimal(const std::string& hexString) {
    std::istringstream iss(hexString);
    int decimalValue;
    iss >> std::hex >> decimalValue;
    return decimalValue;
}

int getSizeChunck(std::string buffer){
    size_t last = buffer.find_first_of("\r\n");
    if (last != std::string::npos){
        // std::cout << "nof found************************\n";
        // std::cout << buffer.substr(0, last) << " ";
        int res = hexToDecimal(buffer.substr(0, last));
        return res;
    }
    return -1;
}

void dataCenter::post(client &clnt, int fd){
    (void)fd;
    (void)clnt;
    std::string directory, file;
    int size = 0;
    // std::cout << clnt.getHeaders()["Transfer-Encoding"] << std::endl;
    static bool isSizeSeted = false;
    if (clnt.getHeaders()["Transfer-Encoding"] == "chunked"){
        if (!isSizeSeted){
            size = getSizeChunck(clnt.getbufferBody());
            clnt.setbufferBody(clnt.getbufferBody().substr(size));
            isSizeSeted = true;
            // std::cout << "size is : " << size << " " << clnt.getHeaders()["Content-Length"] << std::endl;
            std::cout << "new body |" << clnt.getbufferBody() << "| with size : " << size << std::endl;  
        }
        else{
            if (clnt.getbufferBody().find("\r\n") != std::string::npos){
                isSizeSeted = false;
                std::cout << "end of chunked\n";
            }
        }
        // if ()
        // //appendBufferChunk(); // 50000
        // if (size >= 1)
        return ; 
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
    }

    if (!clnt.getFileUpload().is_open()) {
        std::cout << "error opening opload file\n";
        throw clnt.getResponse().setAttributes(500, "text/html");
    }

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