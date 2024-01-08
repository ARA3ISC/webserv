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

class request
{
private:
    startLine_t _startLine;
    std::map<std::string, std::string> _headers;
public:
    request();
    request(const request& rhs);
    request& operator=(const request& rhs);
    ~request();

    void setStartLine(std::string line);
    void setHeaders(std::string line);

    startLine_t getStartLine();
    std::map<std::string, std::string>& getHeaders();
};