#pragma once

# include <iostream>
# include "location.hpp"
# include <map>
# include <vector>
class server
{
private:
    int _locations_count;
	std::vector<std::string> _server_name;
	std::vector<std::string> _listen;
    std::string _root;
    std::map<std::string, std::string> _cgi_path;
    std::vector<std::string> _allow_methods;
    std::vector<std::string> _error_pages;
    std::string _upload;
    int _client_max_body_size;
    std::vector<location> _locations;
//    location* _locations;
public:
    server();
	server(int locations_count);
	server(const server& rhs);
	server& operator=(const server& rhs);
	~server();

    void set_server_name(std::string line, int nbln);
    void set_listen(std::string line, int nbln);

    std::vector<std::string> getListen(){
        return this->_listen;
    }


};
