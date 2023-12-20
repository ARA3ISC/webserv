#include "parsingConfigFile.hpp"
#include "error.hpp"

void    checkValue(std::string line)
{
    (void)line;
    if (getFirstWord(line) == "worker_processes")
        if (!is_digit(getSecondWord(line)))
            throwError("Invalid value");
//    std::cout << getSecondWord(line) << std::endl;
//    std::cout << line << std::endl;
}

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
            || line.at(0) == '#' || line.at(0) == ' ' || line.at(0) == '\t')
            continue;
        if (std::find(mainDirectives.begin(), mainDirectives.end(), getFirstWord(line)) == mainDirectives.end())
        if (line.back() != ';' && ( std::find(mainDirectives.begin(), mainDirectives.begin() + 4, getFirstWord(line)) != mainDirectives.begin() + 4 ) )
        {
            std::string error_line = "syntax error (line: " + std::to_string(lineNb) + ")";
            throwError(error_line);
        }

        if (getFirstWord(line) != mainDirectives[4] && getFirstWord(line) != mainDirectives[5])
        {
            if (countWords(line) != 2)
            {
                std::string error_line = "syntax error (line: " + std::to_string(lineNb) + ")";
                throwError(error_line);
            }
            checkValue(line);
        }
        else
            if (countWords(line) != 1)
            {
                std::string error_line = "syntax error (line: " + std::to_string(lineNb) + ")";
                throwError(error_line);
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

