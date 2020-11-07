#include <iostream>

#include "LexerLib/Lexer.hpp"

int main()
{
	try
	{
		Lexer lexer(std::cin);

		bool needIterate = false;
		do
		{
			const auto [type, lexeme, lineNum, linePos] = lexer.GetLexeme();
			std::cout << "[" << lineNum << ":" << linePos << "] " << lexeme << " <- " << LexemeTypeToString(type) << std::endl;
			needIterate = type != LexemeType::EndOfFile;
		} while (needIterate);
	}
	catch (const std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
	}
}
