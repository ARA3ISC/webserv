#include "../inc/response.hpp"

response::response(){
    this->isReading = true;
    this->lisDir = false;
    this->isResponseSent = false;
    this->isHeaderSend = false;
    this->isCGIFile = false;
    // std::cout << "reading seted\n";
}

response::response(const response &other){
    this->isReading = other.isReading;
    this->lisDir = other.lisDir;
    this->isHeaderSend = other.isHeaderSend;
    this->statusCode = other.statusCode;

    this->path = other.path;
    this->statusMessage = other.statusMessage;
    this->contentType = other.contentType;
    this->content = other.content;
    this->isResponseSent = other.isResponseSent;
    this->isCGIFile = other.isCGIFile;

}

response &response::operator=(const response &other){
    if(this != &other){
        this->isReading = other.isReading;
        this->lisDir = other.lisDir;
        this->isHeaderSend = other.isHeaderSend;
        this->statusCode = other.statusCode;

        this->path = other.path;
        this->statusMessage = other.statusMessage;
        this->contentType = other.contentType;
        this->content = other.content;
        this->isResponseSent = other.isResponseSent;
        this->isCGIFile = other.isCGIFile;
    }
    return *this;
}

response::~response(){
}

bool response::getIsReading(){
    return this->isReading;
}

void response::setIsReading(bool a){
    this->isReading = a;
}

int response::setAttributes(int statusCode, std::string contentType){
    this->statusCode = statusCode;
    this->contentType = contentType;
    this->setIsReading(false);
    
    return 0;
}

void response::setPath(std::string path){
    this->path = path;
}
int response::getStatusCode(){
    return this->statusCode;
}
std::string response::getPath(){
    return this->path;
}
std::string response::getStatusMessage(){
    return this->statusMessage;
}
std::string response::getContentType(){
    return this->contentType;
}

void response::setContent(std::string content){
    // this->lisDir = true;
    this->content= content;
}
std::string response::getContent(){
    return this->content;
}

void response::setLisDir(bool lisDir){
    this->lisDir = lisDir;
}

bool response::getLisDir(){
    return this->lisDir;
}

void response::openFile(std::string path){
    this->filePath.open(path.c_str(), std::ios::in | std::ios::out | std::ios::binary);
    // std::cout << "file to open : " << path << std::endl;
}
#include <unistd.h>
#include <fcntl.h>

void response::openfilePathError(std::string path){
    this->filePathError.open(path.c_str(), std::ios::in | std::ios::out | std::ios::binary);
}

std::fstream &response::getFilePath(){
    return this->filePath;
}

std::fstream &response::getFilePathError(){
    return this->filePathError;
}

bool response::getIsHeaderSend(){
    return this->isHeaderSend;
}

void response::setIsHeaderSend(bool a){
    this->isHeaderSend = a;
}
void response::setIsResponseSent(bool a){
    this->isResponseSent = a;
}
bool response::getIsResponseSent(){
    return this->isResponseSent;
}

void response::setIsCGIFile(bool a){
    this->isCGIFile = a;
}
bool response::getIsCGIFile(){
    return this->isCGIFile;
}
