#include <iostream>
#include <variant>
#include <fstream>
#include <ctime>

#include "../../Lexer/LexerLib/Lexer.hpp"
#include "../CommonLib/Settings.hpp"
#include "Grammar.h"
#include "Generator.hpp"

namespace
{

Grammar LexemizeGrammar(const Grammar& grammar)
{
	auto grammarCopy(grammar);
	if constexpr (Settings::USE_LEXER)
	{
		for (auto& [left, right] : grammarCopy)
		{
			std::transform(right.cbegin(), right.cend(), right.begin(), [](const auto& ch) {
				return IsNonTerminal(ch)
					? ch
					: (IsEndRule(ch) ? ch : LexemeTypeToString(ClassifyLexeme(ch)));
			});
		}
	}
	return grammarCopy;
}

}

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 3)
		{
			throw std::invalid_argument("Invalid arguments, should be <exe> <input_grammar> <table>");
		}

		std::ifstream inputGrammar(argv[1]);
		std::ofstream outputTable(argv[2]);
		if (!inputGrammar.is_open())
		{
			throw std::runtime_error("This file does not exist");
		}

		outputTable << GetTableSLR(LexemizeGrammar(GetGrammar(inputGrammar)));
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
