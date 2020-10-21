#include "SyntacticalAnalyzer.h"

namespace
{

constexpr auto IS_DEBUG = false;

constexpr auto TERMINAL_END_SEQUENCE = "$";

std::string PrecariousLexeme(Lexeme lexeme)
{
	if (IS_DEBUG)
	{
		return lexeme.lexeme;
	}

	if (const auto type = lexeme.type; type != LexemeType::Error)
	{
		return (type != LexemeType::EndOfFile) ? LexemeTypeToString(type) : TERMINAL_END_SEQUENCE;
	}
	else
	{
		return lexeme.lexeme;
	}
}

}

SyntacticalAnalyzer::SyntacticalAnalyzer(std::vector<GuideSetsData> guideSets, std::vector<std::string> headerSymbols, std::vector<LRData> lrData, std::list<Lexeme> sentence)
	:m_guideSets(guideSets)
	,m_headerSymbols(headerSymbols)
	,m_lrData(lrData)
	,m_sentence(sentence)
{
}

void SyntacticalAnalyzer::Run()
{
	auto sentence = m_sentence;
	auto currentData = m_lrData.front();

	std::stack<LRData> stackLRData;
	stackLRData.push(currentData);

	std::stack<Lexeme> lexemesStack;

	while (!sentence.empty() || !stackLRData.empty())
	{
		Lexeme currLexeme = sentence.front();
		sentence.pop_front();
		Symbol symbol = GetSymbolByChInLRData(currentData, currLexeme);

		if (symbol.state == StateSymbol::Shift)
		{
			currentData = m_lrData[symbol.number];
			stackLRData.push(currentData);
			lexemesStack.push(currLexeme);
		}
		else if (symbol.state == StateSymbol::Convolution)
		{
			sentence.push_front(currLexeme);
			const auto rule = m_guideSets[symbol.number];
			for (size_t i = 0; i < (rule.terminals.size() + (symbol.number ? 0 : -1)); ++i)
			{
				if (!stackLRData.empty()) stackLRData.pop();
				if (!lexemesStack.empty()) lexemesStack.pop();
			}
			sentence.push_front({ LexemeType::Error, rule.nonterminal, currLexeme.lineNum, currLexeme.linePos });
			currentData = stackLRData.empty() ? m_lrData.front() : stackLRData.top();
		}
		else if (symbol.state == StateSymbol::Ok)
		{
			std::cout << "Well done!" << std::endl;
			return;
		}
		else if (symbol.state == StateSymbol::None)
		{
			throw std::exception(("SLR(1) failure: none-state at " + std::to_string(currLexeme.lineNum) + ":" + std::to_string(currLexeme.linePos)).c_str());
		}
	}
}

LRData SyntacticalAnalyzer::GetLRDataByCh(std::string currentChar)
{
	if (const auto it = std::find_if(m_lrData.begin(), m_lrData.end(), [&](const LRData& data) {
			return data.ch == currentChar;
		}); it != m_lrData.end())
	{
		return *it;
	}
	throw std::invalid_argument("Error. Wrong character: " + currentChar);
}

Symbol SyntacticalAnalyzer::GetSymbolByChInLRData(const LRData& data, const Lexeme& currLexeme)
{
	const auto headerSymbol = PrecariousLexeme(currLexeme);
	if (const auto it = std::find(m_headerSymbols.cbegin(), m_headerSymbols.cend(), headerSymbol); it != m_headerSymbols.cend())
	{
		const auto index = std::distance(m_headerSymbols.cbegin(), it);
		return data.symbols[index];
	}
	throw std::exception(("Invalid follow for " + headerSymbol + " at " + std::to_string(currLexeme.lineNum) + ":" + std::to_string(currLexeme.linePos)).c_str());
}
