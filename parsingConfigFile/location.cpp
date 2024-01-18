#include "../inc/location.hpp"
#include "../inc/server.hpp"
#include "../inc/utils.hpp"
#include "../inc/error.hpp"
#include "../inc/utils3.hpp"

location::location(): _dir_listing(false)
{
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

void    location::setPath(std::string line, int nbl, server* s)
{
    std::vector<std::string> splited;
    splited = splitBySpace(line);
    removeComment(splited);

    if (splited.size() != 2 && splited.size() != 3)
        throwError("Syntax error", nbl);
    if (splited.size() == 2) {
        this->_path = s->getRoot();
    }
    else
    {
        this->_path = removeLastColon(splited[2]);
        if (this->_path.empty())
            throwError("Syntax error", nbl);
        checkSlash(this->_path);
    }
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

void location::setRoot(std::string line, int nbln)
{
    std::vector<std::string> splited;
    splited = splitBySpace(line);
    if (splited.size() != 2)
        throwError("Syntax error", nbln);
    this->_root = splited[1];
}

void location::setIndexes(std::string line, int nbln) {
    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);

    if (splited.size() == 1)
        throwError("Syntax error", nbln);
    if (hasDuplicates(splited))
    {
        std::cout << "Duplicated symbol (line: " << nbln << ")";
        throw std::runtime_error("");
    }
    for (unsigned long i = 1; i < splited.size(); ++i) {
        this->_index.push_back(splited[i]);
//        std::cout << "added -> " << this->_index[i - 1] << std::endl;
    }
}

std::string location::getRoot()
{
    return this->_root;
}

std::vector<std::string>& location::getIndexes()
{
    return this->_index;
}

location::~location() {}
