#include "inc/parsingConfigFile.hpp"
//#include <sys/qos.h>

int main(int ac, char *av[]) {
    try {
        if (ac > 2)
            throw std::runtime_error("Invalid arguments number");
        else if (ac == 2) {
            startParsing(av[1]);
        } else if (ac == 1) {
            std::cout << "No config file specified\nUsing default one -> configfile.yaml\n";
            startParsing("configfile.yaml");
        }
    }
    catch (std::exception &e) {

        std::cerr << e.what() << std::endl;
    }
    return 0;
}
