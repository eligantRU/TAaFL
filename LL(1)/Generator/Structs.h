#pragma once

#include <vector>
#include <string>

using PairStringBool = std::pair<std::string, bool>;
using PairStringVectorPair = std::pair<std::string, std::vector<PairStringBool>>;
using PairStringString = std::pair<std::string, std::string>;

struct InputData
{
	std::string nonterminal;
	std::vector<std::string> terminals;
};

struct OutputDataGuideSets
{
	std::string nonterminal;
	std::vector<std::string> terminals;
	std::vector<std::string> guideCharacters;
};

struct OutputData
{
	std::string symbol;
	std::vector<std::string> guideCharacters;
	bool isShift = false;
	bool isError = true;
	size_t pointer = 0;
	bool isStack = false;
	bool isEnd = false;
};
