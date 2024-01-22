#pragma once

# include <iostream>
# include <string>
# include <cstdlib>
# include "location.hpp"
# include <map>
# include <vector>

class location;

class server
{
private:
	int _locations_count;
	std::vector<std::string> _server_name;
	std::vector<std::string> _listen;
	std::string _root;
	int _client_max_body_size;
    std::map<int, std::string> _error_pages;
	std::vector<location> _locations;
public:
	server();
	server(int locations_count);
	server(const server& rhs);
	server& operator=(const server& rhs);
	~server();

	void set_server_name(std::string line, int nbln);
	void set_listen(std::string line, int nbln);
	void setRoot(std::string line, int nbln);
    void setMaxBodySize(std::string line, int nbln);
    void setErrorPages(std::string line, int nbln);


	std::vector<std::string>& getServer_names()
	{
		return this->_server_name;
	}

	std::vector<std::string>& getListen(){
		return this->_listen;
	}
	std::string getRoot(){return this->_root;}
    int getMaxBodySize() {return this->_client_max_body_size;}
    std::map<int, std::string>& get_error_pages()
    {
        return this->_error_pages;
    }

	location* createLocation();
	std::vector<location>& getLocations();
	void   addLocation(location &newloc);
};
