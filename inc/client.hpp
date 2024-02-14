#pragma once
#include <map>
#include <string>
#include "../inc/utils.hpp"
#include "../inc/webserv.hpp"
#include "../inc/utils2.hpp"
#include "../inc/response.hpp"
#include <stdio.h>
typedef struct startLine_s
{
    std::string method;
    std::string path;
    std::string http_v;
}           startLine_t;

class client
{
private:
    int _fd;
    int _serverIndex;
    std::string _fullRequest;
    startLine_t _startLine;
    std::map<std::string, std::string> _headers;
    std::string _body;
    bool _headerareloaded;
    response _response;
    std::string _queryString;
    bool isUploadfileOpen;
    std::fstream fileUpload;
    std::string fileUploadName;
    FILE* fileNewUpload;
    std::string bufferBody;
    std::string tempBuffer;
    size_t bufferLen;
    std::string chunk;
    size_t chunkSize;
    size_t fullSize;
    clock_t startTime;
    int locationIndex;
    bool isCgi;

public:
    client();
    client(int serverIndex, int clientFd);
    client(const client& rhs);
    client& operator=(const client& rhs);
    ~client();

    void setFullRequest(const std::string &line);
    void setStartLine(std::string line);
    void setHeaders(std::string line);
    void setBody(std::string line);
    void headersLoaded(bool r);

    std::string getFullRequest();
    startLine_t getStartLine();
    std::map<std::string, std::string>& getHeaders();
    std::string& getBody();
    bool isHeadersLoaded();
    int servIndx();
    void setServIndx(int s);
    int getServIndx(int s);
    response &getResponse();
    int getFd() {return this->_fd;}
    void setResponse(response &res);
    std::string getQueryString();
    void setQueryString(std::string queryString);
    void setIsUploadfileOpen(bool a);

    bool getIsUploadfileOpen();

    void openFileUpload(std::string path);
    std::fstream &getFileUpload();

    void openFileNewUpload(std::string path);
    FILE* getFileNewUpload();

    std::string getbufferBody();
    void setbufferBody(std::string a);

    size_t getbufferLen();
    void setbufferLen(size_t a);
    std::string getTempBuffer();
    void setTempBuffer(std::string a);
    std::string getChunk();
    void setChunk(std::string a);
    size_t getChunkSize();
    void setChunkSize(size_t a);
    void setbufferBody(std::istringstream &a);

    size_t getFullSize();
    void setFullSize(int a);

    clock_t getStartTime();
    void setStartTime(clock_t a);
    std::string getFileUploadName();

    void setLocationIndex(int a);
    int getLocationIndex();

    void setIsCgi(bool a);
    bool getIsCgi();
};
