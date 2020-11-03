#include <iostream>
#include <variant>
#include <fstream>
#include <ctime>

#include "Common.hpp"
#include "Grammar.hpp"
#include "Generator.hpp"

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

		outputTable << GetTableSLR(GetGrammar(inputGrammar));
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
