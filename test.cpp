#include <iostream>
#include <map>

int main() {
    // Create a map
    std::map<int, std::string> myMap;

    // Inserting elements into the map using operator[]
    myMap[1] = "One";
    myMap[2] = "Two";
    myMap[3] = "Three";

    myMap[1] = "One1";
    // Print the map
    std::cout << "Map contents:" << std::endl;
    for (std::map<int, std::string>::iterator it = myMap.begin(); it != myMap.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }

    return 0;
}
