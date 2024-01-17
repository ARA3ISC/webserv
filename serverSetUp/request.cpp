#include "../inc/request.hpp"

request::request() {}

request::request(const request &rhs) {
    this->_startLine = rhs._startLine;
    this->_headers = rhs._headers;
}

request& request::operator=(const request &rhs) {
    if (this != &rhs)
    {
        this->_startLine = rhs._startLine;
        this->_headers = rhs._headers;
    }
    return *this;
}

request::~request() {}

/* end of canonical form */

void request::setFullRequest(std::string line) {
    this->_fullRequest.append(line);
}

void request::setStartLine(std::string line) {
    if (line.at(0) == 32 || line.at(0) == '\t')
        throw std::runtime_error("Bad request 400");
    std::vector<std::string> values = splitBySpace(line);

    if (values.size() != 3)
        throw std::runtime_error("Bad request");
    this->_startLine.method = values[0];
    this->_startLine.path = values[1];
    this->_startLine.http_v = values[2];
}

void request::setHeaders(std::string line) {
    if (line[0] == 32 || line[0] == '\t')
        throw std::runtime_error("Bad request 400");
    std::cout << "[" << line << "]" << std::endl;
    std::vector<std::string> values = splitHeaderBycolon(line);
    if (values.size() != 2) {
        throw std::runtime_error("Bad request!!!");
    }
    this->_headers.insert(std::pair<std::string, std::string>(values[0], values[1]));
}

void    request::setBody(std::string line) {
    std::string body = trimFromBeginning(line, "\r\n\r\n");
    this->_body = body;
}

std::string request::getFullRequest() {
    return this->_fullRequest;
}

startLine_t request::getStartLine() {
    return this->_startLine;
}

std::map<std::string, std::string>& request::getHeaders() {
    return this->_headers;
}

std::string& request::getBody() {
    return this->_body;
}
