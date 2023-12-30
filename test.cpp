#include <iostream>
#include <map>

int main()
{
    std::map<int, int> mp;

    mp.insert(std::pair<int, int>(3, 4));
    mp.insert(std::pair<int, int>(30, 40));

    std::map<int, int>::iterator it = mp.begin();
    it++;
    std::cout << it->first << " - "<< it->second << std::endl;
}