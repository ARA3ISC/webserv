#pragma once
#include <map>
#include <string>
#include "../inc/utils.hpp"
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
    std::string _fullRequest;
    startLine_t _startLine;
    std::map<std::string, std::string> _headers;
    std::string _body;
public:
    client();
    client(const client& rhs);
    client& operator=(const client& rhs);
    ~client();

    void setFullRequest(std::string line);
    void setStartLine(std::string line);
    void setHeaders(std::string line);
    void setBody(std::string line);

    std::string getFullRequest();
    startLine_t getStartLine();
    std::map<std::string, std::string>& getHeaders();
    std::string& getBody();
};