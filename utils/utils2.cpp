/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maneddam <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 16:05:26 by maneddam          #+#    #+#             */
/*   Updated: 2024/02/14 16:05:27 by maneddam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/utils2.hpp"
#include "../inc/utils.hpp"

std::vector<std::string> splitHeaderBycolon(std::string line) {

    std::string key;
    std::string value;

    std::vector<std::string> splited;
    int i = 0;
    while (line[i] && line[i] != ':')
        key += line[i++];

    if (line[i] != ':')
        throw 400;
    else
    {
        if (line[i - 1] == ' ' || line[i - 1] == '\t')
            throw 400;
        i++;
    }
    while (line[i])
        value += line[i++];

    splited.push_back(key);
    splited.push_back(trimStr(value));
    return splited;
}

std::string trimFromBeginning(const std::string& input, const std::string& prefix) {
    size_t pos = input.find(prefix);

    if (pos != std::string::npos) {
        return input.substr(pos + prefix.length());
    } else {
        return input;
    }
}
