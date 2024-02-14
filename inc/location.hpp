#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <iostream>
#include <vector>
#include <map>

#include "parsingConfigFile.hpp"
#include "server.hpp"
class server;



class location
{
private:
    std::string _path;
    bool _dir_listing;
    std::vector<std::string> _allow_methods;
    std::map<std::string, std::string> _cgi_path;
    std::string _root;
    std::vector<std::string> _index;
    bool _auto_index;
    std::string _upload;
    std::string _return;
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
    void setCgiPath(std::string& line, int nbln);
    void setUpload(std::string line, int nbln);
    void setReturn(std::string line, int nbln);




    bool get_dir_listing();
    std::string getPath();


    std::vector<std::string>& getMethods();
    std::string getRoot();
    std::vector<std::string>& getIndexes();
    bool isAutoIndex();
    std::map<std::string, std::string>& getCgiPath() ;
    std::string getUpload() ;
    std::string getReturn();
    void isRootSet(server &s);
    void isMethodSet(int nbline);

};

#endif