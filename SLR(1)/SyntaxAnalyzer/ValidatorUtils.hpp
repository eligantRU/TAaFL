#pragma once

#include "../../Lexer/LexerLib/Lexer.hpp"
#include "../CommonLib/Settings.hpp"

#include "TableUtils.hpp"

namespace
{

std::string PrecariousLexeme(Lexeme lexeme)
{
	if constexpr (!Settings::USE_LEXER)
	{
		return (lexeme.type != LexemeType::EndOfFile) ? lexeme.lexeme : TERMINAL_END_SEQUENCE;
	}
	else if (const auto type = lexeme.type; type != LexemeType::Error)
	{
		return (type != LexemeType::EndOfFile) ? LexemeTypeToString(type) : TERMINAL_END_SEQUENCE;
	}
	else
	{
		return lexeme.lexeme;
	}
}

}

std::vector<Lexeme> GetSentence(std::istream& inputSentence)
{
	Lexer lexer(inputSentence);

	std::vector<Lexeme> lexemes;
	do {} while (lexemes.emplace_back(lexer.GetLexeme()).type != LexemeType::EndOfFile);
	return lexemes;
}

void ValidateSentence(const std::vector<std::string>& characters, const Table<std::optional<std::variant<Shift, Reduce>>>& table, const std::vector<Lexeme>& lexemes)
{
	std::list<Lexeme> sentence(lexemes.cbegin(), lexemes.cend());

	const auto axiom = characters.front();
	std::unordered_map<std::string, size_t> getChIndex;
	for (size_t i = 0; i < characters.size(); ++i)
	{
		getChIndex[characters[i]] = i;
	}

	auto currData = table[0];
	
	std::stack<std::remove_reference<decltype(table)>::type::Row> st;
	st.push(currData);

	while (!sentence.empty() || !st.empty())
	{
		if (st.empty() && (sentence.size() == 2) && (PrecariousLexeme(sentence.front()) == characters.front()) && (PrecariousLexeme(sentence.back()) == characters.back()))
		{
			break;
		}

		auto currLexeme = sentence.front();
		sentence.pop_front();

		if (const auto& cell = currData[getChIndex[PrecariousLexeme(currLexeme)]]; cell)
		{
			std::visit([&table, &currData, &sentence, &st, &currLexeme](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, Shift>)
				{
					currData = st.emplace(table[arg.pointer]);
				}
				else if constexpr (std::is_same_v<T, Reduce>)
				{
					for (size_t i = 0; i < arg.len; ++i) st.pop();
					sentence.push_front(currLexeme);
					sentence.push_front({ LexemeType::Error, "<" + arg.ch + ">", currLexeme.lineNum, currLexeme.linePos }); // TODO: not so Error-state
					currData = st.empty() ? table[0] : st.top();
				}
			}, *cell);
		}
		else
		{
			throw std::runtime_error(std::string("[") + ToString(currLexeme.lineNum) + ":" + ToString(currLexeme.linePos) + "] Unexpected lexeme");
		}
	}
}
