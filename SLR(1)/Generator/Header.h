#pragma once
#include <algorithm>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

const std::string TAB = "\t";
const std::string SPACE = " ";
const std::string DELIMITER = "=>";
const std::string NONTERMINAL_END_SEQUENCE = "e";
const std::string TERMINAL_END_SEQUENCE = "#";

enum class StateGenerator
{
	START,
	EMPTY,
	SHIFT,
	ROLL_UP,
};

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

struct TableData
{
	std::string nonterminal;
	std::string character;
	size_t row = 0;
	size_t position = 0;
};

struct OutputData
{
	TableData tableData;
	std::vector<std::string> transitions;
};

struct Transition
{
	std::vector<TableData> tableDatas;
	StateGenerator state = StateGenerator::EMPTY;
};

struct OutputDataGenerator
{
	TableData tableData;
	std::vector<Transition> transitions;
};