#include <iostream>
#include <variant>
#include <fstream>
#include <ctime>

#include "Common.h"
#include "Grammar.hpp"
#include "Generator.hpp"

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

		const auto grammar = GetGrammar(inputGrammar);
		PrintGrammar(outputGrammar, grammar);

		outputTable << GetTableSLR(grammar);
	}
	catch (const std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
	}
}
