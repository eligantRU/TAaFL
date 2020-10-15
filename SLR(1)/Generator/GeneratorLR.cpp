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

std::vector<std::string> GetFollow(const std::vector<OutputDataGuideSets>& rules, std::string nonTerminal, std::set<std::string> way = {}) // TODO:
{
	std::vector<std::string> result;
	for (const auto& subRule : rules)
	{
		if (auto it = std::find_if(subRule.terminals.cbegin(), subRule.terminals.cend(), [&](std::string_view sv) {
			return sv == nonTerminal;
			}); it != subRule.terminals.cend())
		{
			size_t distance = std::distance(subRule.terminals.cbegin(), it);
			size_t size = subRule.terminals.size() - 1;

			if (const auto bla = (distance <= size)
				? ((distance < size) ? subRule.terminals[distance + 1] : subRule.terminals.back())
				: NONTERMINAL_END_SEQUENCE; bla != nonTerminal)
			{
				result.push_back(bla);

				if ((bla == TERMINAL_END_SEQUENCE) && !way.count(subRule.nonterminal)) // TODO: copy-paste
				{
					auto tmpWay(way);
					tmpWay.insert(subRule.nonterminal);
					const auto tmp = GetFollow(rules, subRule.nonterminal, tmpWay);
					std::copy(tmp.cbegin(), tmp.cend(), std::back_inserter(result));
				}
			}
			else if (!way.count(subRule.nonterminal))
			{
				auto tmpWay(way);
				tmpWay.insert(subRule.nonterminal);
				const auto tmp = GetFollow(rules, subRule.nonterminal, tmpWay);
				std::copy(tmp.cbegin(), tmp.cend(), std::back_inserter(result));
			}
		}
	}
	return Uniqify(result);
}

void GeneratorLR::Generate()
{
	auto transitions = ColdStart();
	TransitionsToTable(transitions);
	auto nextToProcess = GetNextToProcess(transitions);
	transitions.clear();
	
	const auto axiom = m_datas.front().nonterminal;
	for (const auto& next : nextToProcess)
	{
		for (const auto& [ch, pos] : next)
		{
			if (((pos.second + 1) == m_datas[pos.first].terminals.size())
				|| (((pos.second + 2) == m_datas[pos.first].terminals.size()) && IsEndRule(m_datas[pos.first].terminals.back())))
			{ // roll-up
				if (((pos.second + 2) == m_datas[pos.first].terminals.size()))
				{
					transitions[TERMINAL_END_SEQUENCE].insert(pos.first);
					continue;
				}
				const auto follows = GetFollow(m_datas, m_datas[pos.first].nonterminal);
				for (const auto& follow : follows)
				{
					transitions[follow].insert(pos.first);
				}
			}
			else
			{
				transitions[m_datas[pos.first].terminals[pos.second + 1]].insert(std::make_pair(pos.first, pos.second + 1));
				for (const auto& [ch, pos] : GetFirstByNonTerminal(m_datas, m_datas[pos.first].terminals[pos.second + 1]))
				{
					transitions[ch].insert(pos);
				}
			}
		}
		m_mainColumn.push_back(next);
		TransitionsToTable(transitions);
		transitions.clear();
	}
}

std::map<std::string, std::set<std::variant<size_t, std::pair<size_t, size_t>>>> GeneratorLR::ColdStart() const
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
	return transitions;
}

void GeneratorLR::TransitionsToTable(const std::map<std::string, std::set<std::variant<size_t, std::pair<size_t, size_t>>>>& transitions)
{
	static size_t rowNum = 0;
	for (const auto& [k, v] : transitions)
	{
		for (const auto& var : v)
		{
			const auto columnNum = std::distance(m_chars.cbegin(), std::find(m_chars.cbegin(), m_chars.cend(), k));

			std::visit([this, &k, columnNum](auto && arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, size_t>)
				{
					m_table[rowNum][columnNum].insert("R" + std::to_string(arg));
				}
				else if constexpr (std::is_same_v<T, std::pair<size_t, size_t>>)
				{
					m_table[rowNum][columnNum].insert(k + std::to_string(arg.first) + "," + std::to_string(arg.second));
				}
				else
				{
					static_assert(always_false_v<T>, "non-exhaustive visitor!");
				}
			}, var);
		}
	}
	++rowNum;
}

std::set<std::set<std::pair<std::string, std::pair<size_t, size_t>>>> GeneratorLR::GetNextToProcess(const std::map<std::string, std::set<std::variant<size_t, std::pair<size_t, size_t>>>>& transitions) const
{
	std::set<std::set<std::pair<std::string, std::pair<size_t, size_t>>>> nextToProcess;
	for (const auto& transition : transitions)
	{
		std::set<std::pair<std::string, std::pair<size_t, size_t>>> bla;
		const auto& [k, v] = transition;
		for (const auto& var : v)
		{
			std::visit([&k, &bla](auto && arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, size_t>)
				{
					// Do nothing
				}
				else if constexpr (std::is_same_v<T, std::pair<size_t, size_t>>)
				{
					bla.insert(std::make_pair(k, arg));
				}
				else
				{
					static_assert(always_false_v<T>, "non-exhaustive visitor!");
				}
			}, var);
		}
		if (!bla.empty())
		{
			nextToProcess.insert(bla);
		}
	}
	return nextToProcess;
}

void GeneratorLR::Print(std::ostream& output) const
{
	output << "Number" << TAB << "Char" << TAB;
	PrintInfoVector(output, m_chars, TAB);
	output << std::endl;
	for (size_t i = 0; i < m_table.size(); ++i)
	{
		output << i << TAB;
		if (!i)
		{
			output << "Init";
		}
		else
		{
			std::string bla = "[";
			if (i > m_mainColumn.size())
			{
				bla = "[Undef]";
			}
			else
			{
				for (const auto& [ch, pos] : m_mainColumn[i - 1])
				{
					bla += ch + std::to_string(pos.first) + "," + std::to_string(pos.second) + "+";
				}
				bla += "]";
			}

			output << bla;
		}
		output << TAB;
		for (const auto& action : m_table[i])
		{
			std::vector<std::string> bla(action.cbegin(), action.cend());
			PrintInfoVector(output, bla, "+");
			output << "_"; // TODO:
		}
		output << std::endl;
	}
}
