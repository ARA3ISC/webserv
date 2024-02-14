/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils3.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maneddam <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 16:05:29 by maneddam          #+#    #+#             */
/*   Updated: 2024/02/14 16:05:30 by maneddam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/utils3.hpp"

void checkSlash(std::string &path)
{
    if (path.at(0) != '/')
        path.insert(path.begin(), '/');
}

void printEntryMsg()
{
    std::cout << GREEN << "" << std::endl;
    std::cout << "███████╗███████╗██████╗ ██╗   ██╗███████╗██████╗     ██╗███████╗" << std::endl;
    std::cout << "██╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗    ██║██╔════╝" << std::endl;
    std::cout << "███████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝    ██║███████╗" << std::endl;
    std::cout << "╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗    ██║╚════██║" << std::endl;
    std::cout << "███████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║    ██║███████║" << std::endl;
    std::cout << "╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝    ╚═╝╚══════╝" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "██████╗ ██╗   ██╗███╗   ██╗███╗   ██╗██╗███╗   ██╗ ██████╗      " << std::endl;
    std::cout << "██╔══██╗██║   ██║████╗  ██║████╗  ██║██║████╗  ██║██╔════╝      " << std::endl;
    std::cout << "██████╔╝██║   ██║██╔██╗ ██║██╔██╗ ██║██║██╔██╗ ██║██║  ███╗     " << std::endl;
    std::cout << "██╔══██╗██║   ██║██║╚██╗██║██║╚██╗██║██║██║╚██╗██║██║   ██║     " << std::endl;
    std::cout << "██║  ██║╚██████╔╝██║ ╚████║██║ ╚████║██║██║ ╚████║╚██████╔╝     " << std::endl;
    std::cout << "╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚═╝  ╚═══╝╚═╝╚═╝  ╚═══╝ ╚═════╝      " << RESET << std::endl;

}

bool checkMaxBodySize(std::string line)
{
    char *end;
    double n = std::strtod(line.c_str(), &end);
    if (*end)
    {
        if (strcmp(end, "m") != 0 && strcmp(end, "M") != 0 && strcmp(end, "g") != 0
         && strcmp(end, "G") != 0 && strcmp(end, "k") != 0 && strcmp(end, "K") != 0)
            return 1;
        else
        {
            if (strcmp(end, "m") == 0 || strcmp(end, "M") == 0)
                if (n > 2000)
                    return 1;
            if (strcmp(end, "g") == 0 || strcmp(end, "G") == 0)
                if (n > 2)
                    return 1;
            if (strcmp(end, "k") == 0 || strcmp(end, "K") == 0)
                if (n > 20000)
                    return 1;
        }
    }
    return 0;
}

long getValue(std::string value)
{
    char *end;
    long n = std::strtol(value.c_str(), &end, 10);
    return n;
}
