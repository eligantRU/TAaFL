#include <iostream>
#include <fstream>

#include "Lexer.hpp"

int main()
{
	try
	{		
		std::ifstream strm("input.txt");

		Lexer lexer(strm);

		while (true)
		{
			constexpr auto wrapped = false;
			constexpr auto wrapper = wrapped ? "'" : "";
			std::cout << wrapper << lexer.GetLexeme() << wrapper << std::endl;
		}
	}
	catch (const std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown error" << std::endl;
	}
}
