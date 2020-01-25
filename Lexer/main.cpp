#include <iostream>

#include "Lexer.hpp"

int main()
{
	try
	{
		Lexer lexer(std::cin);

		bool needIterate = false;
		do
		{
			const auto [type, lexeme] = lexer.GetLexeme();
			std::cout << lexeme << " <- " << LexemeTypeToString(type) << std::endl;
			needIterate = type != LexemeType::EndOfFile;
		} while (needIterate);
	}
	catch (const std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
	}
}
