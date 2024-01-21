#pragma once
#include <map>
#include <string>
#include "../inc/utils.hpp"
#include "../inc/webserv.hpp"
#include "../inc/utils2.hpp"
typedef struct startLine_s
{
    std::string method;
    std::string path;
    std::string http_v;
}           startLine_t;

class client
{
private:
    size_t _serverIndex;
    std::string _fullRequest;
    startLine_t _startLine;
    std::map<std::string, std::string> _headers;
    std::string _body;
    bool _headerareloaded;
public:
    client();
    client(size_t serverIndex);
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
    size_t servIndx();
    void setServIndx(size_t s);
};
