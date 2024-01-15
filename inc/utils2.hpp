#pragma once
#include <iostream>
#include <vector>

std::vector<std::string> splitHeaderBycolon(std::string line);

std::string trimFromBeginning(const std::string& input, const std::string& prefix);