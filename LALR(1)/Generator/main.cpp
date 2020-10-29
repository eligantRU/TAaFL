#include <iostream>
#include <variant>
#include <fstream>
#include <ctime>

#include "Common.h"
#include "Grammar.hpp"
#include "Generator.hpp"

namespace
{

void PrintGrammar(std::ostream& output, const std::vector<Rule>& rules)
{
	for (const auto& rule : rules)
	{
		output << rule.left << SPACE << SEPARATOR << SPACE;
		PrintVector(output, rule.right, SPACE);
		output << std::endl;
	}
}

void GenerateParser(std::istream& inputGrammar, std::ostream& outputGrammar, std::ostream& outputTable)
{
	auto grammar = GetGrammar(inputGrammar);
	do
	{
		try
		{
			outputTable << GetTableSLR(grammar);
		}
		catch (const ShiftReduceConflict& ex)
		{
			grammar = LALR2SLR(grammar, ex.ConflictRuleNum());
			continue;
		}
		PrintGrammar(outputGrammar, grammar); // TODO: for other students only
		break;
	} while (true);
}

}

int main(int argc, char* argv[])
{	
	try
	{
		if (argc != 4)
		{
			throw std::invalid_argument("Invalid arguments, should be <exe> <input_grammar> <output_grammar> <table>");
		}

		std::ifstream inputGrammar(argv[1]);
		std::ofstream outputGrammar(argv[2]);
		std::ofstream outputTable(argv[3]);
		if (!inputGrammar.is_open())
		{
			std::cerr << "This file does not exist" << std::endl;
			return 1;
		}

		GenerateParser(inputGrammar, outputGrammar, outputTable);
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Uncaught exception" << std::endl;
	}
}
