#include "GeneratorSLR.h"

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

std::vector<std::vector<std::variant<std::set<std::pair<size_t, size_t>>, size_t>>> MakeTable(size_t width, size_t height)
{
	std::vector<std::vector<std::variant<std::set<std::pair<size_t, size_t>>, size_t>>> result;
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

}

GeneratorSLR::GeneratorSLR(const std::vector<OutputDataGuideSets>& datas)
	:m_datas(datas)
	,m_chars(GetUniqueCharacters(datas))
	,m_table(MakeTable(m_chars.size(), m_datas.size()))
{
	Generate();
}

void GeneratorSLR::Generate()
{
	auto transitions = ColdStart();
	TransitionsToTable(transitions);
	auto nextToProcess = GetNextToProcess();
	transitions.clear();

	const auto axiom = m_datas.front().nonterminal;
	for (const auto& next : nextToProcess)
	{
		for (const auto& pos : next)
		{
			if (((pos.second + 1) == m_datas[pos.first].terminals.size())
				|| (((pos.second + 2) == m_datas[pos.first].terminals.size()) && IsEndRule(m_datas[pos.first].terminals.back())))
			{ // roll-up
				if (((pos.second + 2) == m_datas[pos.first].terminals.size()))
				{
					transitions[TERMINAL_END_SEQUENCE] = pos.first;
					continue;
				}
				const auto follows = GetFollow(m_datas, m_datas[pos.first].nonterminal);
				for (const auto& follow : follows)
				{
					transitions[follow] = pos.first;
				}
			}
			else
			{
				std::get<0>(transitions[m_datas[pos.first].terminals[pos.second + 1]]).insert(std::make_pair(pos.first, pos.second + 1));
				for (const auto& [ch, pos] : GetFirstByNonTerminal(m_datas, m_datas[pos.first].terminals[pos.second + 1]))
				{
					std::get<0>(transitions[ch]).insert(pos);
				}
			}
		}

		m_mainColumn.emplace_back(next);
		TransitionsToTable(transitions);
		transitions.clear();
	}
}

std::map<std::string, std::variant<std::set<std::pair<size_t, size_t>>, size_t>> GeneratorSLR::ColdStart() const
{
	const auto axiom = m_datas.front().nonterminal;

	std::map<std::string, std::variant<std::set<std::pair<size_t, size_t>>, size_t>> transitions;
	for (size_t i = 0; i < m_datas.size(); ++i)
	{
		const auto& [left, right, _] = m_datas[i];
		if (left != axiom)
		{
			continue;
		}

		if (right.size() == 1)
		{
			transitions[right.front()] = i;
		}
		else
		{
			std::get<0>(transitions[right.front()]).insert(std::make_pair(i, 0));
		}

		if (IsNonterminal(right.front()))
		{
			for (const auto& [ch, pos] : GetFirstByNonTerminal(m_datas, right.front()))
			{
				std::get<0>(transitions[right.front()]).insert(pos);
			}
		}
	}
	return transitions;
}

void GeneratorSLR::TransitionsToTable(const std::map<std::string, std::variant<std::set<std::pair<size_t, size_t>>, size_t>>& transitions)
{
	static size_t rowNum = 0;
	for (const auto& [k, v] : transitions)
	{
		std::visit([this, &k](auto && arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, size_t>)
			{
				const auto columnNum = std::distance(m_chars.cbegin(), std::find(m_chars.cbegin(), m_chars.cend(), k));
				m_table[rowNum][columnNum] = arg;
			}
			else if constexpr (std::is_same_v<T, std::set<std::pair<size_t, size_t>>>)
			{
				for (const auto& pos : arg)
				{
					const auto ch = m_datas[pos.first].terminals[pos.second];
					const auto columnNum = std::distance(m_chars.cbegin(), std::find(m_chars.cbegin(), m_chars.cend(), ch));
					std::get<0>(m_table[rowNum][columnNum]).insert(pos);
				}
			}
			else
			{
				static_assert(always_false_v<T>, "non-exhaustive visitor!");
			}
		}, v);
	}
	++rowNum;
}

std::set<std::set<std::pair<size_t, size_t>>> GeneratorSLR::GetNextToProcess() const
{
	std::set<std::set<std::pair<size_t, size_t>>> nextToProcess;
	for (const auto& row : m_table)
	{
		for (const auto& cell : row)
		{
			if (std::visit([this](auto&& arg) {
					using T = std::decay_t<decltype(arg)>;
					if constexpr (std::is_same_v<T, size_t>)
					{
						return false;
					}
					else if constexpr (std::is_same_v<T, std::set<std::pair<size_t, size_t>>>)
					{
						return !arg.empty() && (std::find(m_mainColumn.cbegin(), m_mainColumn.cend(), arg) == m_mainColumn.cend());
					}
					else
					{
						static_assert(always_false_v<T>, "non-exhaustive visitor!");
					}
				}, cell))
			{
				nextToProcess.insert(std::get<0>(cell));
			}
		}
	}
	return nextToProcess;
}

void GeneratorSLR::Print(std::ostream& output) const
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
			std::string bla;
			if (i > m_mainColumn.size())
			{
				bla = "[Undef]";
			}
			else
			{
				bla = "[";
				for (const auto& pos : m_mainColumn[i - 1])
				{
					const auto ch = m_datas[pos.first].terminals[pos.second];
					bla += ch + std::to_string(pos.first) + "," + std::to_string(pos.second) + "+";
				}
				bla += "]";
			}

			output << bla;
		}
		output << TAB;
		for (const auto& action : m_table[i])
		{
			output << std::visit([this](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, size_t>)
				{
					return "R" + std::to_string(arg);
				}
				else if constexpr (std::is_same_v<T, std::set<std::pair<size_t, size_t>>>)
				{
					if (arg.empty())
					{
						return std::string("-");
					}

					std::string bla;
					for (const auto& pos : arg)
					{
						const auto ch = m_datas[pos.first].terminals[pos.second];
						bla += ch + std::to_string(pos.first) + "," + std::to_string(pos.second) + "+";
					}
					return bla;
				}
				else
				{
					static_assert(always_false_v<T>, "non-exhaustive visitor!");
				}
			}, action);
			output << " "; // TODO:
		}
		output << std::endl;
	}
}
