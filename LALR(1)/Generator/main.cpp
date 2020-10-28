#include <iostream>
#include <variant>
#include <fstream>
#include <ctime>

#include "Common.h"
#include "Grammar.hpp"
#include "Generator.hpp"

std::vector<Rule> FixGrammar(const std::vector<Rule>& baseGrammar, const std::pair<size_t, size_t>& pos)
{
	std::vector<Rule> grammar(baseGrammar);

	const auto conflictNonTerminal = baseGrammar[pos.first].left;

	std::vector<std::string> nonTerminals;
	for (auto& [left, right]: grammar)
	{
		for (auto& ch : right)
		{
			ch = (ch == conflictNonTerminal) ? nonTerminals.emplace_back("<" + GetRandomString() + ">") : ch;
		}
	}

	const auto grammarCopy(grammar);
	for (const auto& [left, right] : grammarCopy)
	{
		if (left != conflictNonTerminal)
		{
			continue;
		}

		for (const auto& nonTerminal : nonTerminals)
		{
			grammar.push_back({ nonTerminal, right });
		}
	}
	
	decltype(grammar) result;
	std::copy_if(grammar.cbegin(), grammar.cend(), std::back_inserter(result), [&conflictNonTerminal](const auto& rule) {
		return rule.left != conflictNonTerminal;
	});
	return result;
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

		std::srand(unsigned(std::time(nullptr)));

		auto grammar = GetGrammar(inputGrammar);
		do
		{
			try
			{
				outputTable << GetTableSLR(grammar);
			}
			catch (const ShiftReduceConflict& ex)
			{
				grammar = FixGrammar(grammar, ex.Position());
				continue;
			}
			PrintGrammar(outputGrammar, grammar);
			break;
		} while (true);
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}
}
