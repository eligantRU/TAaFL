#pragma once

#include <set>
#include <map>

#include "../CommonLib/Settings.hpp"
#include "../CommonLib/Common.hpp"
#include "../CommonLib/Table.hpp"

#include "Grammar.h"
#include "ShiftReduceConflict.h"

namespace
{

template<class> inline constexpr bool always_false_v = false;

// TODO: expicit constructor with contract check & immutable model
struct Shift
{
	std::string ch;
	std::shared_ptr<std::vector<std::set<std::pair<size_t, size_t>>>> mainColumn;
	std::set<std::pair<size_t, size_t>> value;

	operator std::string()
	{
		if constexpr (Settings::USE_OPTIMIZED_TABLE)
		{
			return std::to_string(1 + std::distance(mainColumn->cbegin(), std::find(mainColumn->cbegin(), mainColumn->cend(), value)));
		}
		else
		{
			std::string res;
			for (size_t i = 0; i < value.size(); ++i)
			{
				auto it = value.cbegin();
				std::advance(it, i);
				const auto& [row, col] = *it;
				res += (i ? "|" : "") + ToString(row) + "," + ToString(col);
			}
			return ch + "(" + res + ")";
		}
	}
};

struct Reduce
{
	size_t value;
	std::string ch;
	size_t len;

	operator std::string()
	{
		if constexpr (Settings::USE_OPTIMIZED_TABLE)
		{
			return ch + "|" + ToString(len);
		}
		else
		{
			return "R" + ToString(value);
		}
	}
};

std::vector<std::pair<std::string, std::pair<size_t, size_t>>> GetFirst(const std::vector<Rule>& grammar, const Rule& processingRule, const std::set<Rule>& processedRules = {})
{
	if (processedRules.count(processingRule))
	{
		return {};
	}

	std::vector<std::pair<std::string, std::pair<size_t, size_t>>> result;
	
	const auto firstProcessingRight = processingRule.right.front();
	const auto rulePos = std::distance(grammar.cbegin(), std::find(grammar.cbegin(), grammar.cend(), processingRule));

	if (IsNonTerminal(firstProcessingRight))
	{
		for (const auto& rule : grammar)
		{
			if (rule.left == firstProcessingRight)
			{
				if (rule != processingRule)
				{
					auto tmpProcessedRules(processedRules);
					tmpProcessedRules.insert(processingRule);

					const auto first = GetFirst(grammar, rule, tmpProcessedRules);
					std::copy(first.cbegin(), first.cend(), std::back_inserter(result));
				}
				result.emplace_back(rule.left, std::make_pair(rulePos, 0));
			}
		}
	}
	else
	{
		result = { std::make_pair(firstProcessingRight, std::make_pair(rulePos, 0)) };
	}
	return Uniqify(result);
}

std::vector<std::pair<std::string, std::pair<size_t, size_t>>> GetFirstByNonTerminal(const std::vector<Rule>& grammar, const std::string& nonTerminal)
{
	std::vector<std::pair<std::string, std::pair<size_t, size_t>>> result;
	for (const auto& rule : grammar)
	{
		if (rule.left != nonTerminal)
		{
			continue;
		}
		auto first = GetFirst(grammar, rule);
		std::copy(first.cbegin(), first.cend(), std::back_inserter(result));
	}
	return Uniqify(result);
}

std::vector<std::string> GetFollow(const std::vector<Rule>& grammar, std::string nonTerminal, std::set<std::string> way = {})
{
	std::vector<std::string> result;
	for (const auto& [left, right] : grammar)
	{
		if (auto it = std::find_if(right.cbegin(), right.cend(), [&](std::string_view sv) {
				return sv == nonTerminal;
			}); it != right.cend())
		{
			for (size_t i = 0; i < right.size(); ++i)
			{
				if (right[i] == nonTerminal)
				{
					size_t distance = i;
					size_t size = right.size();

					if (distance < (size - 1))
					{
						result.push_back(right[distance + 1]);
						const auto first = GetFirstByNonTerminal(grammar, right[distance + 1]);
						for (const auto& [ch, pos] : first)
						{
							result.push_back(ch);
						}
					}
					else if (distance == (size - 1))
					{
						if (!way.count(left))
						{
							auto tmpWay(way);
							tmpWay.insert(left);
							const auto tmp = GetFollow(grammar, left, tmpWay);
							std::copy(tmp.cbegin(), tmp.cend(), std::back_inserter(result));
						}
					}
				}
			}
		}
	}
	return Uniqify(result);
}

std::vector<std::string> GetUniqueCharacters(const std::vector<Rule>& grammar)
{
	std::set<std::string> result;
	for (const auto& [left, right] : grammar)
	{
		result.emplace(left);
		result.insert(right.cbegin(), right.cend());
	}
	std::vector<std::string> vecResult(result.cbegin(), result.cend());
	vecResult.erase(std::remove_if(vecResult.begin(), vecResult.end(), [&grammar](const auto& ch) {
		const auto axiom = grammar.front().left;
		return (ch == TERMINAL_END_SEQUENCE) || (ch == axiom);
	}), vecResult.end());
	vecResult.insert(vecResult.begin(), grammar.front().left);
	vecResult.insert(vecResult.end(), TERMINAL_END_SEQUENCE);
	return vecResult;
}

std::map<std::string, std::variant<std::set<std::pair<size_t, size_t>>, size_t>> ColdStart(const std::vector<Rule>& grammar)
{
	const auto axiom = grammar.front().left;

	std::map<std::string, std::variant<std::set<std::pair<size_t, size_t>>, size_t>> transitions;
	for (size_t i = 0; i < grammar.size(); ++i)
	{
		const auto& [left, right] = grammar[i];
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

		if (const auto firstRight = right.front(); IsNonTerminal(firstRight))
		{
			const auto first = GetFirstByNonTerminal(grammar, firstRight);
			for (const auto& [ch, pos] : first)
			{
				std::get<0>(transitions[firstRight]).insert(pos);
			}
		}
	}
	return transitions;
}

void TransitionsToTable(const std::vector<Rule>& grammar, const std::vector<std::string> chars,
	std::shared_ptr<std::vector<std::set<std::pair<size_t, size_t>>>> mainColumn,
	const std::map<std::string, std::variant<std::set<std::pair<size_t, size_t>>, size_t>>& transitions,
	Table<std::optional<std::variant<Shift, Reduce>>>& table, size_t& rowNum)
{
	table.AddRow(std::remove_reference<decltype(table)>::type::Row(chars.size()));
	for (const auto& [k, v] : transitions)
	{
		std::visit([&k, &grammar, &chars, &mainColumn, &table, &rowNum](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, size_t>)
			{
				const auto columnNum = std::distance(chars.cbegin(), std::find(chars.cbegin(), chars.cend(), k));
				table[rowNum][columnNum] = Reduce{ arg, grammar[arg].left, grammar[arg].right.size() };
			}
			else if constexpr (std::is_same_v<T, std::set<std::pair<size_t, size_t>>>)
			{
				for (const auto& pos : arg)
				{
					const auto ch = grammar[pos.first].right[pos.second];
					const auto columnNum = std::distance(chars.cbegin(), std::find(chars.cbegin(), chars.cend(), ch));
					
					if (auto& cell = table[rowNum][columnNum]; cell)
					{
						std::get<Shift>(*cell).value.insert(pos);
					}
					else
					{
						cell = Shift{ ch, mainColumn, { pos } };
					}
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

std::set<std::set<std::pair<size_t, size_t>>> GetNextToProcess(const Table<std::optional<std::variant<Shift, Reduce>>>& table,
	std::shared_ptr<std::vector<std::set<std::pair<size_t, size_t>>>> mainColumn)
{
	std::set<std::set<std::pair<size_t, size_t>>> nextToProcess;
	for (const auto& row : table)
	{
		for (const auto& cell : row)
		{
			if (cell && std::visit([&mainColumn](auto&& arg) {
					using T = std::decay_t<decltype(arg)>;
					if constexpr (std::is_same_v<T, Reduce>)
					{
						return false;
					}
					else if constexpr (std::is_same_v<T, Shift>)
					{
						return !arg.value.empty() && (std::find(mainColumn->cbegin(), mainColumn->cend(), arg.value) == mainColumn->cend());
					}
					else
					{
						static_assert(always_false_v<T>, "Non-exhaustive visitor!");
					}
				}, *cell))
			{
				nextToProcess.insert(std::get<0>(*cell).value);
			}
		}
	}
	return nextToProcess;
}

void ProcessNextShift(const std::vector<Rule>& grammar, const std::pair<size_t, size_t>& pos,
	std::map<std::string, std::variant<std::set<std::pair<size_t, size_t>>, size_t>>& transitions)
{
	if (((pos.second + 2) == grammar[pos.first].right.size()))
	{
		transitions[TERMINAL_END_SEQUENCE] = pos.first;
		return;
	}

	const auto follows = GetFollow(grammar, grammar[pos.first].left);
	for (const auto& follow : follows)
	{
		if (std::holds_alternative<std::set<std::pair<size_t, size_t>>>(transitions[follow]))
		{
			if (!std::get<std::set<std::pair<size_t, size_t>>>(transitions[follow]).empty()) // TODO: does it can be empty? Should be fixed
			{
				throw ShiftReduceConflict(pos.first);
			}
		}
		transitions[follow] = pos.first;
	}
}

void ProcessNextReduce(const std::vector<Rule>& grammar, const std::pair<size_t, size_t>& pos,
	std::map<std::string, std::variant<std::set<std::pair<size_t, size_t>>, size_t>>& transitions)
{
	if (std::holds_alternative<size_t>(transitions[grammar[pos.first].right[pos.second + 1]]))
	{
		throw ShiftReduceConflict(pos.first);
	}
	std::get<0>(transitions[grammar[pos.first].right[pos.second + 1]]).insert(std::make_pair(pos.first, pos.second + 1));
	const auto first = GetFirstByNonTerminal(grammar, grammar[pos.first].right[pos.second + 1]);
	for (const auto& [firstCh, firstPos] : first)
	{
		if (std::holds_alternative<size_t>(transitions[firstCh]))
		{
			throw ShiftReduceConflict(firstPos.first);
		}
		std::get<0>(transitions[firstCh]).insert(firstPos);
	}
}

void ProcessNext(const std::set<std::pair<size_t, size_t>>& next, const std::vector<Rule>& grammar,
	std::map<std::string, std::variant<std::set<std::pair<size_t, size_t>>, size_t>>& transitions)
{
	for (const auto& pos : next)
	{
		if (((pos.second + 1) == grammar[pos.first].right.size())
			|| (((pos.second + 2) == grammar[pos.first].right.size()) && IsEndRule(grammar[pos.first].right.back())))
		{
			ProcessNextShift(grammar, pos, transitions);
		}
		else
		{
			ProcessNextReduce(grammar, pos, transitions);
		}
	}
}

}

Table<std::optional<std::variant<Shift, Reduce>>> GetTableSLR(const std::vector<Rule>& grammar)
{
	const auto chars = GetUniqueCharacters(grammar);
	Table<std::optional<std::variant<Shift, Reduce>>> table(chars, [](const auto& cell) {
		return cell
			? std::visit([](auto&& arg) {
					return ToString(arg);
				}, *cell)
			: "-";
	});
	size_t rowNum = 0;
	auto mainColumn = std::make_shared<std::vector<std::set<std::pair<size_t, size_t>>>>(); // TODO:

	auto transitions = ColdStart(grammar);
	TransitionsToTable(grammar, chars, mainColumn, transitions, table, rowNum);
	transitions.clear();

	auto nextToProcess = GetNextToProcess(table, mainColumn);
	do
	{
		for (const auto& next : nextToProcess)
		{
			ProcessNext(next, grammar, transitions);

			mainColumn->emplace_back(next);
			TransitionsToTable(grammar, chars, mainColumn, transitions, table, rowNum);
			transitions.clear();
		}
		nextToProcess = GetNextToProcess(table, mainColumn);
	} while (!nextToProcess.empty());
	return table;
}
