/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils3.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maneddam <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 16:04:58 by maneddam          #+#    #+#             */
/*   Updated: 2024/02/14 16:04:59 by maneddam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS3_HPP
# define UTILS3_HPP

#include <iostream>
#include "utils.hpp"
#define GREEN "\033[92m"
#define RESET "\033[0m"
#include <cstdlib>
#include <cstring>

void checkSlash(std::string &path);
bool checkMaxBodySize(std::string line);
void printEntryMsg();
long getValue(std::string value);

#endif
