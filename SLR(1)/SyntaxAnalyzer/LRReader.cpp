#include <sstream>

#include "LRReader.h"

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

LRReader::LRReader()
{
}

void LRReader::ReadGuideSets(std::istream& fileGuideSetsInput)
{
	std::string line;
	while (std::getline(fileGuideSetsInput, line))
	{
		std::istringstream iss(line);
		std::string str;

		GuideSetsData guideSetsData;
		bool isTerminal = false;

		while (iss >> str)
		{
			if (str == "/")
			{
				break;
			}
			if (str == "=>")
			{
				isTerminal = true;
			}
			else if (isTerminal)
			{
				guideSetsData.terminals.push_back(str);
			}
			else
			{
				guideSetsData.nonterminal = str;
			}
		}
		m_guideSets.push_back(guideSetsData);
	}
}

void LRReader::ReadSentence(std::istream& fileSentenceInput)
{
	auto lexemes = GetAllLexemes(fileSentenceInput);
	lexemes.erase(lexemes.end() - 1);

	for (const auto& lexeme : lexemes)
	{
		m_sentence.push_back(lexeme);
	}
	m_sentence.push_back({LexemeType::EndOfFile, "$", 0, 0});
}

void LRReader::ReadTable(std::istream& fileTableInput)
{
	std::string line;

	std::getline(fileTableInput, line);
	InitHeaderSymbols(line);

	while (std::getline(fileTableInput, line))
	{
		std::istringstream iss(line);
		m_lrData.push_back({ ParseNumber(GetString(iss)), GetString(iss), GetSymbols(iss) });
	}
	m_lrData.front().symbols.front().state = StateSymbol::Ok;
}

std::vector<GuideSetsData> LRReader::GetGuideSets()
{
	return m_guideSets;
}

std::vector<std::string> LRReader::GetHeaderSymbols()
{
	return m_headerSymbols;
}

std::vector<LRData> LRReader::GetLRData()
{
	return m_lrData;
}

std::list<Lexeme> LRReader::GetSentence()
{
	return m_sentence;
}

void LRReader::InitHeaderSymbols(std::string line)
{
	std::istringstream iss(line);
	std::string str;
	for (size_t i = 0; i <= 1; i++)
	{
		iss >> str;
	}
	while (iss >> str)
	{
		m_headerSymbols.push_back(str);
	}
}

std::vector<Symbol> LRReader::GetSymbols(std::istringstream& iss)
{
	std::vector<Symbol> symbols;
	std::string str;
	while (iss >> str)
	{
		symbols.push_back(ParseSymbol(str));
	}

	return symbols;
}
