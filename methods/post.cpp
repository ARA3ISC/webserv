#include "../inc/dataCenter.hpp"

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