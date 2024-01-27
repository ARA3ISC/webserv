#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include <iostream>
#include <fstream>

class response
{
private:
    bool isReading;
    bool lisDir;
    bool isHeaderSend;
    bool isResponseSent;
    bool isCGIFile;
    int statusCode;

    std::fstream filePath;
    std::fstream filePathError;
    std::string path;
    std::string statusMessage;
    std::string contentType;
    std::string content;

public:
    response();
    response(const response &other);
    response &operator=(const response &other);
    ~response();
    
    void setIsReading(bool a);
    int setAttributes(int statusCode, std::string contentType);
    void setPath(std::string path);
    void setContent(std::string content);
    void setLisDir(bool lisDir);
    void setIsHeaderSend(bool isHeaderSend);
    void setIsResponseSent(bool a);
    void openFile(std::string path);
    void openfilePathError(std::string path);
    void setIsCGIFile(bool a);
    
    std::string getPath();
    std::string getStatusMessage();
    std::string getContentType();
    std::string getContent();
    bool getIsReading();
    int getStatusCode();
    bool getIsHeaderSend();
    bool getLisDir();
    std::fstream &getFilePath();
    std::fstream &getFilePathError();
    bool getIsResponseSent();
    bool getIsCGIFile();


};

#endif