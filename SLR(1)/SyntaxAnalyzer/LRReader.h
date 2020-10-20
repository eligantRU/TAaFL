#pragma once

#include <fstream>
#include <sstream>
#include <list>

#include "Helper.h"
#include "LRData.h"
#include "../../Lexer/Lexer.hpp"


class LRReader
{
public:
	LRReader();
	void ReadGuideSets(std::istream& fileGuideSetsInput);
	void ReadSentence(std::istream& fileSentenceInput);
	void ReadTable(std::istream& fileTableInput);
	std::vector<GuideSetsData> GetGuideSets();
	std::vector<LRData> GetLRData();
	std::list<Lexeme> GetSentence();
	std::vector<std::string> GetHeaderSymbols();

private:
	std::vector<Symbol> GetSymbols(std::istringstream& iss);
	void InitHeaderSymbols(std::string line);

	std::vector<GuideSetsData> m_guideSets;
	std::vector<std::string> m_headerSymbols;
	std::vector<LRData> m_lrData;
	std::list<Lexeme> m_sentence;
};
