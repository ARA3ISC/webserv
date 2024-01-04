#pragma once

#include <iostream>
#include <vector>

#include "parsingConfigFile.hpp"

class location
{
private:
    std::string _path;
    bool _dir_listing;
    std::vector<std::string> _allow_methods;
    std::string _root;
    std::string _index;
    bool _auto_index;
public:
    location();
    location(const location& rhs);
    location& operator=(const location& rhs);
    ~location();


    void setPath(std::string line, int nbl);
    void set_dir_listing(std::string line, int nb);
    void setMethods(std::string line, int nbln);


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
};
