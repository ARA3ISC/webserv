/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maneddam <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 16:04:52 by maneddam          #+#    #+#             */
/*   Updated: 2024/02/14 16:04:53 by maneddam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>


int is_empty(const char *s);
std::string getFirstWord(std::string line);
std::string getSecondWord(std::string line);
int countWords(const std::string& str);
bool is_digit(std::string arg);

std::vector<std::string> splitBySpace(const std::string& input);
std::string trimStr(const std::string& str);
bool isNaN(std::string str);
void    removeComment(std::vector<std::string>& splited);
std::string removeLastColon(std::string& input);

std::vector<std::string> splitBy(const std::string& input, char delimiter);

#endif
