#include "webserv.hpp"
#include "parsingConfigFile.hpp"

int main(int ac, char *av[])
{
    (void)av;
    try {
        if (ac > 2)
            throw std::runtime_error("Invalid arguments number");
        else if (ac == 2)
        {
            startParsing(av[1]);
        }
        else if (ac == 1)
            exit(0);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}