#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <regex>
#include <stack>

#include "../../Lexer/LexerLib/Lexer.hpp"
#include "../CommonLib/Table.hpp"

#include "TableUtils.hpp"
#include "ValidatorUtils.hpp"

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 3)
		{
			throw std::invalid_argument("Invalid arguments, should be <exe> <table> <sentence>");
		}

		std::ifstream inputTable(argv[1]);
		std::ifstream inputSentence(argv[2]);

		if (!inputTable.is_open() || !inputSentence.is_open())
		{
			throw std::runtime_error("This file does not exist");
		}
		
		const auto [headerSymbols, table] = GetTable(inputTable);
		const auto sentence = GetSentence(inputSentence);

		ValidateSentence(headerSymbols, table, sentence);
		std::cout << "Well done!" << std::endl;
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
