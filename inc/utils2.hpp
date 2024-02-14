/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maneddam <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 16:04:54 by maneddam          #+#    #+#             */
/*   Updated: 2024/02/14 16:04:55 by maneddam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <vector>

std::vector<std::string> splitHeaderBycolon(std::string line);

std::string trimFromBeginning(const std::string& input, const std::string& prefix);
