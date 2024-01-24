#include "../inc/client.hpp"

client::client() {
    this->_headerareloaded = false;
}

client::client(size_t serverIndex, int clientFd) {
    this->_headerareloaded = false;
    this->_serverIndex = serverIndex;
    this->_fd = clientFd;
}

client::client(const client &rhs) {
    this->_fd = rhs._fd;
    this->_startLine = rhs._startLine;
    this->_headers = rhs._headers;
    this->_body = rhs._body;
    this->_headerareloaded= rhs._headerareloaded;
    this->_serverIndex = rhs._serverIndex;
    this->_fullRequest = rhs._fullRequest;
}

client& client::operator=(const client &rhs) {
    if (this != &rhs)
    {
        this->_fd = rhs._fd;

        this->_startLine = rhs._startLine;
        this->_headers = rhs._headers;
        this->_body = rhs._body;
        this->_headerareloaded= rhs._headerareloaded;
        this->_serverIndex = rhs._serverIndex;
        this->_fullRequest = rhs._fullRequest;
    }
    return *this;
}

client::~client() {}

/* end of canonical form */

void client::setFullRequest(const std::string &line) {
//    std::cout << this->_fullRequest.size() << std::endl;
    this->_fullRequest.append(line);
}

void client::setStartLine(std::string line) {
    if (line.at(0) == 32 || line.at(0) == '\t')
        throw 400;
    std::vector<std::string> values = splitBySpace(line);

    if (values.size() != 3)
        throw std::runtime_error("Bad client");
//    std::cout << values[0] << std::endl;
    if (values[0] != "GET" && values[0] != "POST" && values[0] != "DELETE")
        throw 405;
    this->_startLine.method = values[0];
    this->_startLine.path = values[1];
    this->_startLine.http_v = values[2];
}

void client::setHeaders(std::string line) {
    if (line[0] == 32 || line[0] == '\t')
        throw 400;
    std::vector<std::string> values = splitHeaderBycolon(line);
    if (values.size() != 2) {
        throw std::runtime_error("Bad client!!!");
    }
    this->_headers.insert(std::pair<std::string, std::string>(values[0], values[1]));
}

void    client::setBody(std::string line) {
    std::string body = trimFromBeginning(line, "\r\n\r\n");
    this->_body = body;
}

void    client::headersLoaded(bool r) {
    this->_headerareloaded = r;
}

std::string client::getFullRequest() {
    return this->_fullRequest;
}

bool client::isHeadersLoaded() {
    return this->_headerareloaded;
}

startLine_t client::getStartLine() {
    return this->_startLine;
}

std::map<std::string, std::string>& client::getHeaders() {
    return this->_headers;
}

std::string& client::getBody() {
    return this->_body;
}

size_t client::servIndx() {return this->_serverIndex;}

void client::setServIndx(size_t serverIndex) {
    this->_serverIndex = serverIndex;
}

