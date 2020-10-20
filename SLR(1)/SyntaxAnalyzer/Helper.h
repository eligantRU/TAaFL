#pragma once

#include <sstream>
#include <stack>
#include <string>

#include "LRData.h"

std::string GetString(std::istringstream& iss);
size_t ParseNumber(const std::string str);
Symbol ParseSymbol(std::string str);
StateSymbol ParseState(char fCh);
bool IsNonterminal(const std::string str);
