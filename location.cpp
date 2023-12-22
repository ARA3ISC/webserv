#include "location.hpp"

location::location() {}
location::location(const location &rhs) {
    this->_path = rhs._path;
    this->_dir_listing = rhs._dir_listing;
    this->_allow_methods = rhs._allow_methods;
    this->_root = rhs._root;
    this->_index = rhs._index;
    this->_auto_index = rhs._auto_index;
}

location &location::operator=(const location &rhs) {
    if (&rhs != this)
    {
        this->_path = rhs._path;
        this->_dir_listing = rhs._dir_listing;
        this->_allow_methods = rhs._allow_methods;
        this->_root = rhs._root;
        this->_index = rhs._index;
        this->_auto_index = rhs._auto_index;
    }
    return *this;
}
location::~location() {}