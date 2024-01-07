#include "inc/location.hpp"
#include "inc/server.hpp"
#include "inc/utils.hpp"
#include "inc/error.hpp"

location::location(): _dir_listing(false)
{
//    server s;
//    this->_allow_methods = s.getMethods();
}
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

void    location::setPath(std::string line, int nbl)
{
    std::vector<std::string> splited;
    splited = splitBySpace(line);
    removeComment(splited);


    if (splited.size() != 2 && splited.size() != 3)
        throwError("Syntax error", nbl);
    if (splited.size() == 2)
        this->_path = "/";
    else
        this->_path = splited[2];
}

void location::set_dir_listing(std::string line, int nbl)
{
    std::vector<std::string> splited;
    splited = splitBySpace(line);
    removeComment(splited);


    if (splited.size() != 2 || (splited[1] != "on" && splited[1] != "off") )
        throwError("Syntax error", nbl);

    if (splited[1] == "on")
        this->_dir_listing = true;
    else
        this->_dir_listing = false;
}

void    location::setMethods(std::string line, int nbln) {
    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);


    if (splited.size() == 1)
        throwError("Syntax error", nbln);
    if (invalidMethod(splited))
    {
        std::cout << "Invalid method (line: " << nbln << ")";
        throw std::runtime_error("");
    }
    if (hasDuplicates(splited))
    {
        std::cout << "Duplicated symbol (line: " << nbln << ")";
        throw std::runtime_error("");
    }
    this->_allow_methods.clear();
    for (unsigned long i = 1; i < splited.size(); ++i) {
        this->_allow_methods.push_back(splited[i]);
    }
}

location::~location() {}
