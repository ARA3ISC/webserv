#include <iostream>
#include <vector>
#include <algorithm>

class server {
public:
    // Example server class with a property
    std::string name;

    // Add other server class members and methods here
};

int main() {
    // Create a vector of server objects
    std::vector<server> myServers;

    // Populate the vector with server objects
    myServers.push_back({"Server1"});
    myServers.push_back({"Server2"});
    myServers.push_back({"Server3"});

    // Reverse the vector
    std::reverse(myServers.begin(), myServers.end());

    // Print the reversed vector
	for (unsigned long i = 0; i < myServers.size(); i++)
	{
        std::cout << myServers[i].name << std::endl;
		/* code */
	}



    return 0;
}
