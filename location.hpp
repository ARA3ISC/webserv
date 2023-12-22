#pragma once

#include <iostream>
#include <vector>
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
};