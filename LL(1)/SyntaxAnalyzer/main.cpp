#include "Impl.hpp"

namespace
{

std::vector<Lexeme> GetAllLexemes(std::istream & input)
{
	std::vector<Lexeme> lexemes;

	Lexer lexer(input);
	bool needIterate = false;
	do
	{
		const auto lexeme = lexer.GetLexeme();
		lexemes.push_back(lexeme);
		needIterate = (lexeme.type != LexemeType::EndOfFile);
	} while (needIterate);
	return lexemes;
}

}

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		std::cout << "Invalid input, should be: <exe> <table> <sentence> <output>" << std::endl;
		return 1;
	}

	std::ifstream table(argv[1]);
	std::ifstream sentence(argv[2]);
	std::ofstream output(argv[3]);

	if (!table.is_open() || !sentence.is_open())
	{
		std::cerr << "This file does not exist" << std::endl;
		return 1;
	}

	try
	{
		std::vector<InputTableData> tableIn;
		std::vector<OutputTableData> tableOut;

		std::vector<Lexeme> lexemes = GetAllLexemes(sentence);
		if (!lexemes.empty()) // TODO:
		{
			if (auto& last = lexemes.back(); last.type == LexemeType::EndOfFile)
			{
				last.lexeme = END_CHAIN;
			}
		}
		
		InitInputTable(table, tableIn);
		MakeProcess(tableIn, tableOut, lexemes);
		PrintResult(output, tableOut);
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}
}
