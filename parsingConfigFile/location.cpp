/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlarabi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 16:05:10 by maneddam          #+#    #+#             */
/*   Updated: 2024/02/19 13:39:38 by rlarabi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/location.hpp"
#include "../inc/server.hpp"
#include "../inc/utils.hpp"
#include "../inc/error.hpp"
#include "../inc/utils3.hpp"

location::location(): _dir_listing(false), _auto_index(false)
{
}
location::location(const location &rhs) {

    this->_path = rhs._path;
    this->_dir_listing = rhs._dir_listing;
    this->_allow_methods = rhs._allow_methods;
    this->_root = rhs._root;
    this->_index = rhs._index;
    this->_cgi_path = rhs._cgi_path;
    this->_auto_index = rhs._auto_index;
    this->_upload = rhs._upload;
    this->_return = rhs._return;

}

location &location::operator=(const location &rhs) {
    if (&rhs != this)
    {
        this->_path = rhs._path;
        this->_dir_listing = rhs._dir_listing;
        this->_allow_methods = rhs._allow_methods;
        this->_root = rhs._root;
        this->_index = rhs._index;
        this->_cgi_path = rhs._cgi_path;
        this->_auto_index = rhs._auto_index;
        this->_upload = rhs._upload;
        this->_return = rhs._return;

    }
    return *this;
}

void    location::setPath(std::string line, int nbl, server* s)
{
    std::vector<std::string> splited;
    splited = splitBySpace(line);
    removeComment(splited);

    if (splited.size() != 2 && splited.size() != 3)
    {
        throwError("Syntax error", nbl);
    }
    if (splited.size() == 2) {
        this->_path = "/";
    }
    else
    {
        this->_path = removeLastColon(splited[2]);
        if (this->_path.empty() || this->_path.at(0) != '/' || this->_path[_path.size() -1] != '/')
            throwError("Syntax error", nbl);
    }

    if (s->isLocationAlreadyExist(this->_path))
        throwError("Duplicated location error", nbl);

}

void location::set_dir_listing(std::string line, int nbl)
{
    std::vector<std::string> splited;
    splited = splitBySpace(line);
    removeComment(splited);

    if (splited.size() != 2 || (splited[1] != "on" && splited[1] != "off") )
    {
        throwError("Syntax error", nbl);
    }

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
    {
        throwError("Syntax error", nbln);
    }
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
    if (splited.size() != 2 || splited[1].at(0) != '.' || splited[1].at(1) != '/')
        throwError("Syntax error", nbln);
    this->_root = splited[1];

}

void location::setIndexes(std::string line, int nbln) {
    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);

    if (splited.size() == 1)
    {
        throwError("Syntax error", nbln);
    }
    if (hasDuplicates(splited))
    {
        std::cout << "Duplicated symbol (line: " << nbln << ")";
        throw std::runtime_error("");
    }
    for (unsigned long i = 1; i < splited.size(); ++i) {
        this->_index.push_back(splited[i]);
    }
}

void location::setAutoIndex(std::string line, int nbln)
{
    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);
    if (splited.size() != 2 || (splited[1] != "on" && splited[1] != "off"))
    {
        throwError("Syntax error", nbln);
    }

    if (splited[1] == "on")
        this->_auto_index = true;
}

void location::setCgiPath(std::string& line, int nbln)
{
    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);

    if (splited.size() != 3)
    {
        throwError("Syntax error", nbln);
    }
    if (this->_cgi_path.find(splited[1]) != this->_cgi_path.end()) {
        throwError("Duplicated symbol", nbln);
    }
    this->_cgi_path.insert(std::pair<std::string, std::string>(splited[1], splited[2]));
}

void    location::setUpload(std::string line, int nbln)
{
    std::vector<std::string> splited;

    splited = splitBySpace(line);
    removeComment(splited);


    if (splited.size() != 2 || splited[1].at(0) != '/')
    {
        throwError("Syntax error", nbln);
    }
    this->_upload = splited[1];
}

void location::setReturn(std::string line, int nbln) {
    std::vector<std::string> splited;
    splited = splitBySpace(line);
    if (splited.size() != 2)
        throwError("Syntax error", nbln);
    this->_return = splited[1];
}

std::string location::getRoot()
{
    return this->_root;
}

std::vector<std::string>& location::getIndexes()
{
    return this->_index;
}

bool location::isAutoIndex()
{
    return this->_auto_index;
}

std::map<std::string, std::string>& location::getCgiPath()
{
    return this->_cgi_path;
}
bool location::get_dir_listing()
{
    return this->_dir_listing;
}
std::string location::getPath()
{
    return this->_path;
}


std::vector<std::string>& location::getMethods()
{
    return this->_allow_methods;
}
std::string location::getUpload() {return this->_upload;}
std::string location::getReturn() {return this->_return;}
location::~location() {}
