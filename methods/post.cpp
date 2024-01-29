#include "../inc/dataCenter.hpp"

void dataCenter::post(client clnt, int fd){
    (void)fd;
//----> code from GET
    std::string directory, file;
    
    server srv = getWebserv().getServers()[clnt.servIndx()];    

    //split the directory and file fron the client request
    splitPath(clnt.getStartLine().path, directory, file); 
    
    //get the index of the location 
    int j = getLocationRequested(srv.getLocations(), directory);

    if (j == -1)
         throw clnt.getResponse().setAttributes(404, "text/html");

    //is method allowed in config file
    if(isMethodAllowed(srv.getLocations()[j].getMethods(), "GET"))
        throw clnt.getResponse().setAttributes(405, "text/html");
    
    // seting the querystring fromthe complite path of the request 
    std::string tmp = srv.getLocations()[j].getRoot() + clnt.getStartLine().path;
    std::size_t queryIndex = tmp.find_last_of("?");
    
    if (queryIndex != std::string::npos)
        clnt.setQueryString(tmp.substr(queryIndex + 1));
    else
        clnt.setQueryString("");
//----> end code from GET

    std::size_t lastExtention = clnt.getHeaders()["Content-Type"].find_first_of("/");
    std::stringstream a;
    a << "output";
    a << time(0);
    a << ".";
    a <<  clnt.getHeaders()["Content-Type"].substr(lastExtention + 1);
    
    std::cout << "file : " << a.str() << std::endl; 
    std::ofstream outputFile(a.str().c_str(), std::ios::binary);

    if (outputFile.is_open()) {
        outputFile.write(clnt.getBody().c_str(), clnt.getBody().size());
        outputFile.close();
    } else {
        std::cerr << "Error opening the output file." << std::endl;
    }
    
    std::cout << "format file : " << clnt.getHeaders()["Content-Type"] << std::endl;
    std::cout << "***"<<  clnt.getBody().size() << "*************" << clnt.getHeaders()["Content-Length"] << "*****" << std::endl;
}