#include "parsingConfigFile.hpp"

void    checkMainContext(std::ifstream &obj)
{
    std::string line;
    int lineNb = 0;
    std::vector<std::string> mainDirectives;
    mainDirectives.push_back("user");
    mainDirectives.push_back("worker_processes");
    mainDirectives.push_back("error_log");
    mainDirectives.push_back("pid");
    mainDirectives.push_back("events");
    mainDirectives.push_back("http");

    while (getline(obj, line))
    {
        lineNb++;
        if (line.empty() || is_empty(line.c_str())
            || line[0] == '#' || line[0] == ' ' || line[0] == '\t')
            continue;
        if (std::find(mainDirectives.begin(), mainDirectives.end(), getFirstWord(line)) == mainDirectives.end())
            throw std::runtime_error("Invalid directive");
        if (line.back() != ';' && ( std::find(mainDirectives.begin(), mainDirectives.begin() + 4, getFirstWord(line)) != mainDirectives.begin() + 4 ) )
        {
            std::string error_line = "syntax error (line: " + std::to_string(lineNb) + ")";
            throw std::runtime_error(error_line.c_str());
        }

//        std::cout << line << std::endl;
    }
}

void    startParsing(std::string filename)
{
    std::ifstream obj(filename);
    if (obj.is_open())
    {
        checkMainContext(obj);
        obj.close();
    }
    else
        throw std::runtime_error("Cannot open file");
}

