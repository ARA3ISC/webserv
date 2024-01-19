#pragma once

#include <iostream>
#include <vector>

#include "parsingConfigFile.hpp"
#include "server.hpp"
class server;



class location
{
private:
    std::string _path;
    bool _dir_listing;
    std::vector<std::string> _allow_methods;
    std::string _root;
    std::vector<std::string> _index;
    bool _auto_index;
public:
    location();
    location(const location& rhs);
    location& operator=(const location& rhs);
    ~location();


    void setPath(std::string line, int nbl, server* s);
    void set_dir_listing(std::string line, int nb);
    void setMethods(std::string line, int nbln);
    void setRoot(std::string line, int nbln);
    void setIndexes(std::string line, int nbln);
    void setAutoIndex(std::string line, int nbln);


    bool get_dir_listing()
    {
        return this->_dir_listing;
    }
    std::string getPath()
    {
        return this->_path;
    }
//    std::vector<std::string>& getMethods() {return this->_allow_methods;}


    std::vector<std::string>& getMethods()
    {
        return this->_allow_methods;
    }
    std::string getRoot();
    std::vector<std::string>& getIndexes();
    bool isAutoIndex();
};
