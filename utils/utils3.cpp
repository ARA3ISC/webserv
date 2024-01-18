#include "../inc/utils3.hpp"

//std::string validPath(std::string path)
//{
//    std::string validpath;
//
//    validpath = removeLastColon(path);
//    return validpath;
//}

void checkSlash(std::string &path)
{
    if (path.at(0) != '/')
        path.insert(path.begin(), '/');
}