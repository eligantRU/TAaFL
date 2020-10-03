#pragma once

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <set>

const std::string TAB = "\t";
const std::string SPACE = " ";
const std::string DELIMITER = "=>";
const std::string NONTERMINAL_END_SEQUENCE = "e";
const std::string TERMINAL_END_SEQUENCE = "$";

bool IsCheckUniqueness(const std::vector<std::string>& vec, const std::string& str);
bool IsNonterminal(const std::string& str);
bool IsEmptyRule(const std::string& str);
bool IsEndRule(const std::string& str);
size_t GetRandomNumber(size_t min, size_t max);
std::string SubstrNonterminal(std::string str);
std::string GetRandomString();
void PrintInfoVector(std::ostream& output, const std::vector<std::string>& vec, std::string str);
