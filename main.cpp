#include "inc/parsingConfigFile.hpp"
#include "inc/setUpServer.hpp"
//#include <sys/qos.h>

void m(int ac, char *av[])
{
    try {
        if (ac > 2)
            throw std::runtime_error("Invalid arguments number");
        else if (ac == 2)
        {
            startParsing(av[1]);
        }
        else if (ac == 1)
        {
            startParsing("configfile.yaml");
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

//    return 0;
}

int main(int ac, char **av)
{
    m(ac, av);
//    system("leaks webserv");
}
