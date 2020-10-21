#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <set>

#include "../../Lexer/Lexer.hpp"
#include "Helper.h"
#include "LRData.h"

class SyntacticalAnalyzer
{
public:
	SyntacticalAnalyzer(std::vector<GuideSetsData> guideSets, std::vector<std::string> headerSymbols, std::vector<LRData> lrData, std::list<Lexeme> sentence);
	void Run();

private:
	LRData GetLRDataByCh(std::string currentChar);
	Symbol GetSymbolByChInLRData(const LRData& data, const Lexeme& currLexeme);

	std::vector<GuideSetsData> m_guideSets;
	std::vector<std::string> m_headerSymbols;
	std::vector<LRData> m_lrData;
	std::list<Lexeme> m_sentence;
};