#include <iostream>
#include <fstream>

#include "Lexer.hpp"

int main()
{
	try
	{		
		std::ifstream strm("input.txt");

		Lexer lexer(strm);

		bool needIterate = false;
		do
		{
			constexpr auto wrapped = false;
			constexpr auto wrapper = wrapped ? "'" : "";
			const auto [type, lexeme] = lexer.GetLexeme();
			std::cout << LexemeTypeToString(type) << " " << wrapper << lexeme << wrapper << std::endl;
			needIterate = type != LexemeType::EndOfFile;
		} while (needIterate);
	}
	catch (const std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
	}
}
