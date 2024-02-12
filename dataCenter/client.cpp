#include "../inc/client.hpp"

client::client() {
    this->timeOut = 0;
    this->_headerareloaded = false;
    this->isUploadfileOpen = false;
}

client::client(int serverIndex, int clientFd) {
    this->timeOut = 0;
    this->isCgi = false;
    this->_headerareloaded = false;
    this->_serverIndex = serverIndex;
    this->_fd = clientFd;
    this->bufferLen = 0;
    this->fullSize = 0;
}

client::client(const client &rhs) {
    this->_fd = rhs._fd;
    this->_startLine = rhs._startLine;
    this->_headers = rhs._headers;
    this->_body = rhs._body;
    this->_headerareloaded= rhs._headerareloaded;
    this->_serverIndex = rhs._serverIndex;
    this->_fullRequest = rhs._fullRequest;
    this->_response = rhs._response;
    this->isUploadfileOpen = rhs.isUploadfileOpen;
    this->bufferBody = rhs.bufferBody;
    this->tempBuffer = rhs.tempBuffer;
    this->bufferLen = rhs.bufferLen;
    this->chunk = rhs.chunk;
    this->chunkSize = rhs.chunkSize;
    this->fileNewUpload = rhs.fileNewUpload;
    this->fullSize = rhs.fullSize;
    this->startTime = rhs.startTime;
    this->fileUploadName = rhs.fileUploadName;
    this->locationIndex = rhs.locationIndex;
    this->isCgi = rhs.isCgi;
    this->timeOut = rhs.timeOut;
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
        this->_response = rhs._response;
        this->isUploadfileOpen = rhs.isUploadfileOpen;
        this->bufferBody = rhs.bufferBody;
        this->tempBuffer = rhs.tempBuffer;
        this->bufferLen = rhs.bufferLen;
        this->chunk = rhs.chunk;
        this->chunkSize = rhs.chunkSize;
        this->fileNewUpload = rhs.fileNewUpload;
        this->fullSize = rhs.fullSize;
        this->startTime = rhs.startTime;
        this->fileUploadName = rhs.fileUploadName;
        this->locationIndex = rhs.locationIndex;
        this->isCgi = rhs.isCgi;
        this->timeOut = rhs.timeOut;

    }
    return *this;
}

client::~client() {
    resetTimeOut();
}

/* end of canonical form */

void client::setFullRequest(const std::string &line) {
    this->_fullRequest.append(line);
}

void client::setStartLine(std::string line) {
    if (line.at(0) == 32 || line.at(0) == '\t')
        throw 400;
    std::vector<std::string> values = splitBySpace(line);

    if (values.size() != 3)
        throw 400;
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
        throw 400;
    }
    this->_headers.insert(std::pair<std::string, std::string>(values[0], values[1]));
}

void    client::setBody(std::string line) {
    std::string body = trimFromBeginning(line, "\r\n\r\n");
    this->_body = body;
    // this->setbufferBody(body);

    // std::cout << "size reded " << body.size() << std::endl;

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

int client::servIndx() {return this->_serverIndex;}

void client::setServIndx(int serverIndex) {
    this->_serverIndex = serverIndex;
}

response &client::getResponse(){
    return this->_response;
}

void client::setResponse(response &res){
    this->_response = res;
}

std::string client::getQueryString(){
    return this->_queryString;
}

void client::setQueryString(std::string queryString){
    this->_queryString = queryString;
}
void client::setIsUploadfileOpen(bool a){
    this->isUploadfileOpen = a;
}
bool client::getIsUploadfileOpen(){
    return this->isUploadfileOpen;
}
void client::openFileUpload(std::string path){
    this->fileUploadName = path;
    this->fileUpload.open(path.c_str(), std::ios::out | std::ios::binary);
}
std::fstream &client::getFileUpload(){
    return this->fileUpload;
}
std::string client::getbufferBody(){
    return this->bufferBody;
}
void client::setbufferBody(std::string a){
    this->bufferBody = a;
}

void client::setbufferBody(std::istringstream &a){
    this->bufferBody = a.str();
}

size_t client::getbufferLen(){
    return this->bufferLen;
}
void client::setbufferLen(size_t a){
    this->bufferLen = a;
}
std::string client::getTempBuffer(){
    return this->tempBuffer;
}
void client::setTempBuffer(std::string a){
    this->tempBuffer = a;
}
std::string client::getChunk(){
    return this->chunk;
}
void client::setChunk(std::string a){
    if (a.empty())
        this->chunk = a;
    else
        this->chunk.append(a);
}
size_t client::getChunkSize(){
    return this->chunkSize;
}
void client::setChunkSize(size_t a){
    this->chunkSize = a;
}
void client::openFileNewUpload(std::string path){
    this->fileNewUpload = fopen(path.c_str() , "wb");
}
FILE* client::getFileNewUpload(){
    return this->fileNewUpload;
}

size_t client::getFullSize(){
    return this->fullSize;
}

void client::setFullSize(int a){
    this->fullSize += a;
}

clock_t client::getStartTime(){
    return this->startTime;
}

void client::setStartTime(clock_t a){
    this->startTime = a;
}

void client::resetTimeOut(){
    this->timeOut = 0;
}

std::string client::getFileUploadName(){
    return this->fileUploadName;
}

void client::setLocationIndex(int a){
    this->locationIndex = a;
}

int client::getLocationIndex(){
    return this->locationIndex;
}

void client::setIsCgi(bool a){
    this->isCgi = a;
}
bool client::getIsCgi(){
    return this->isCgi;
}
