#include "GeneratorLR.h"

#include <iostream>

namespace
{

template<class> inline constexpr bool always_false_v = false;

std::vector<std::string> GetUniqueCharacters(const std::vector<OutputDataGuideSets>& datas)
{
	std::set<std::string> result;
	for (const auto& [left, right, guide] : datas)
	{
		result.emplace(left);
		result.insert(right.cbegin(), right.cend());
	}
	std::vector<std::string> vecResult(result.cbegin(), result.cend());
	vecResult.erase(std::remove_if(vecResult.begin(), vecResult.end(), [&datas](const auto& ch) {
		return (ch == TERMINAL_END_SEQUENCE) || (ch == datas.front().nonterminal);
	}), vecResult.end());
	vecResult.insert(vecResult.begin(), datas.front().nonterminal);
	vecResult.insert(vecResult.end(), TERMINAL_END_SEQUENCE);
	return vecResult;
}

std::vector<std::vector<std::set<std::string>>> MakeTable(size_t width, size_t height)
{
	std::vector<std::vector<std::set<std::string>>> result;
	for (size_t i = 0; i < height; ++i)
	{
		std::vector<std::vector<std::string>> tmp(width);
		result.emplace_back(width);
	}
	return result;
}

template <class T>
T Uniqify(const T & c)
{
	std::set bla(c.cbegin(), c.cend());
	return { bla.cbegin(), bla.cend() };
}

std::vector<std::pair<std::string, std::pair<size_t, size_t>>> GetFirst(const std::vector<OutputDataGuideSets> & rules, OutputDataGuideSets processingRule)
{
	std::vector<std::pair<std::string, std::pair<size_t, size_t>>> result;
	
	const auto firstProcessingRight = processingRule.terminals.front();
	const auto rulePos = std::distance(rules.cbegin(), std::find_if(rules.cbegin(), rules.cend(), [&processingRule](const auto& rule) {
		return (rule.nonterminal == processingRule.nonterminal) && (rule.terminals == processingRule.terminals) && (rule.guideCharacters == processingRule.guideCharacters);
	}));

	if (IsNonterminal(firstProcessingRight))
	{
		for (const auto& rule : rules)
		{
			if (rule.nonterminal == firstProcessingRight)
			{
				const auto bla = GetFirst(rules, rule);
				std::copy(bla.cbegin(), bla.cend(), std::back_inserter(result));
				result.emplace_back(rule.nonterminal, std::make_pair(rulePos, 0));
			}
		}
	}
	else
	{
		result = { std::make_pair(firstProcessingRight, std::make_pair(rulePos, 0)) };
	}
	return Uniqify(result);
}

std::vector<std::pair<std::string, std::pair<size_t, size_t>>> GetFirstByNonTerminal(const std::vector<OutputDataGuideSets> & rules, const std::string& nonTerminal)
{
	std::vector<std::pair<std::string, std::pair<size_t, size_t>>> result;
	for (const auto& rule : rules)
	{
		if (rule.nonterminal != nonTerminal)
		{
			continue;
		}
		auto bla = GetFirst(rules, rule);
		std::copy(bla.cbegin(), bla.cend(), std::back_inserter(result));
	}
	return Uniqify(result);
}

}

GeneratorLR::GeneratorLR(const std::vector<OutputDataGuideSets>& datas)
	:m_datas(datas)
	,m_chars(GetUniqueCharacters(datas))
	,m_table(MakeTable(m_chars.size(), m_datas.size()))
{
	Generate();
}

void GeneratorLR::Generate()
{
	const auto axiom = m_datas.front().nonterminal;

	std::map<std::string, std::set<std::variant<size_t, std::pair<size_t, size_t>>>> transitions;
	for (size_t i = 0; i < m_datas.size(); ++i)
	{
		const auto& [left, right, _] = m_datas[i];
		if (left != axiom)
		{
			continue;
		}

		if (right.size() == 1)
		{
			transitions[right.front()].insert(i);
		}
		else
		{
			transitions[right.front()].insert(std::make_pair(i, 0));
		}

		if (IsNonterminal(right.front()))
		{
			for (const auto& [ch, pos] : GetFirstByNonTerminal(m_datas, right.front()))
			{
				transitions[ch].insert(pos);
			}
		}
	}

	for (const auto& [k, v] : transitions)
	{
		for (const auto& var : v)
		{
			const auto column = std::distance(m_chars.cbegin(), std::find(m_chars.cbegin(), m_chars.cend(), k));

			std::visit([this, column](auto && arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, size_t>)
				{
					m_table[0][column].insert("R" + std::to_string(arg));
				}
				else if constexpr (std::is_same_v<T, std::pair<size_t, size_t>>)
				{
					m_table[0][column].insert("S" + std::to_string(arg.first) + "," + std::to_string(arg.second));
				}
				else
				{
					static_assert(always_false_v<T>, "non-exhaustive visitor!");
				}
			}, var);
		}
	}
}

void GeneratorLR::Print(std::ostream& output)
{
	output << "Number" << TAB << "Char" << TAB;
	PrintInfoVector(output, m_chars, TAB);
	output << std::endl;
	for (size_t i = 0; i < m_table.size(); ++i)
	{
		output << i << TAB << (i ? "TODO" : "Init") << TAB;
		for (const auto& action : m_table[i])
		{
			std::vector<std::string> bla(action.cbegin(), action.cend());
			PrintInfoVector(output, bla, "+");
			output << "_"; // TODO:
		}
		output << std::endl;
	}
}
