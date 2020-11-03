#pragma once

#include <sstream>

#include "Common.hpp"
#include "../../Lexer/Lexer.hpp"

namespace
{

std::string GetRandomString()
{
	static size_t callCounter = 0;
	return NONTERMINAL_GEN_PREFIX + ToString(callCounter++);
}

}

bool IsNonTerminal(const std::string_view& sv)
{
	return (sv.size() > 2) && sv.front() == '<' && sv.back() == '>';
}

bool IsEmptyRule(const std::string_view& sv)
{
	return sv == NONTERMINAL_END_SEQUENCE;
}

bool IsEndRule(const std::string_view& sv)
{
	return sv == TERMINAL_END_SEQUENCE;
}

std::string GenerateNonTerminal()
{
	return "<" + GetRandomString() + ">";
}

void PrintVector(std::ostream& output, const std::vector<std::string>& vec, const std::string& separator)
{
	if (vec.empty())
	{
		return;
	}

	if (vec.size() > 1)
	{
		std::copy(vec.cbegin(), vec.cend() - 1, std::ostream_iterator<std::string>(output, separator.c_str()));
	}
	output << vec.back();
}

std::tuple<std::vector<Rule>, std::unordered_set<std::string>, std::unordered_set<std::string>> GetRules(std::istream& input)
{
	std::vector<Rule> rules;
	std::unordered_set<std::string> nonTerminals;
	std::unordered_set<std::string> terminals;

	std::string line;
	while (std::getline(input, line))
	{
		const auto tokens = Tokenize(line);
		if (tokens.size() < 3)
		{
			throw std::logic_error("Invalid rule");
		}
		if (!IsNonTerminal(tokens[0]))
		{
			throw std::logic_error("Left side of the rule should be non-terminal");
		}
		if (tokens[1] != SEPARATOR)
		{
			throw std::logic_error("Left side of the rule should be separated from the right side by separator");
		}
		rules.push_back({ tokens.front(), { tokens.cbegin() + 2, tokens.cend() } });
		nonTerminals.insert(tokens[0]);
		std::for_each(tokens.cbegin() + 2, tokens.cend(), [&terminals](const std::string_view& sv) {
			if (!IsNonTerminal(sv) && !IsEmptyRule(sv) && !IsEndRule(sv))
			{
				terminals.emplace(sv);
			}
		});
	}
	return std::make_tuple(rules, nonTerminals, terminals);
}

void ValidateRules(const std::vector<Rule>& rules, const std::unordered_set<std::string>& nonTerminals)
{
	for (const auto& [left, right]: rules)
	{
		if (IsEmptyRule(right.front()) && (right.size() > 1))
		{
			throw std::logic_error("Empty rule cannot contains another symbols");
		}
		
		for (const auto& el : right)
		{
			if (IsNonTerminal(el))
			{
				if (!nonTerminals.count(el))
				{
					throw std::logic_error("Unknown non-terminal " + el);
				}
				if (el.substr(0, strlen(NONTERMINAL_GEN_PREFIX)) == NONTERMINAL_GEN_PREFIX)
				{
					throw std::logic_error(std::string(NONTERMINAL_GEN_PREFIX) + "-prefix cannot be used in non-terminals' names");
				}
			}
		}
	}
}

void ValidateGrammar(const std::vector<Rule>& rules)
{
	if (rules.empty())
	{
		throw std::logic_error("Empty grammar");
	}
}

bool IsReplenishedGrammar(const std::vector<Rule>& rules)
{
	for (const auto& rule : rules)
	{
		if (const auto it = std::find(rule.right.cbegin(), rule.right.cend(), rules.front().left);
			it != rule.right.cend())
		{
			return false;
		}
	}
	return true;
}

void MakeReplenished(std::vector<Rule>& rules, std::unordered_set<std::string>& nonTerminals)
{
	if (IsReplenishedGrammar(rules))
	{
		return;
	}

	const auto randomNonterminal = GenerateNonTerminal();
	nonTerminals.insert(randomNonterminal);
	rules.insert(rules.begin(), { randomNonterminal, std::vector<std::string>{ rules.front().left } });
}

std::vector<std::vector<std::string>> Combine(const std::vector<std::string>& rule, const std::unordered_set<std::string>& epsables, const std::vector<std::vector<std::string>>& prevs = {}, size_t pos = 0)
{
	if (rule.empty())
	{
		throw std::exception("Rule cannot be empty");
	}
	if (pos == rule.size())
	{
		return prevs;
	}

	const auto curr = rule[pos];
	const auto isEpsilable = epsables.find(curr) != epsables.cend();
	
	using T = std::vector<std::vector<std::string>>;
	if (pos == 0)
	{
		return Combine(rule, epsables, isEpsilable ? T{ { curr }, {} } : T{ { curr } }, pos + 1);
	}
	else
	{
		std::vector<std::vector<std::string>> newPrevs = isEpsilable ? T{ prevs.cbegin(), prevs.cend() } : T{};
		for (auto prev : prevs)
		{
			prev.push_back(curr);
			newPrevs.push_back(prev);
		}
		return Combine(rule, epsables, newPrevs, pos + 1);
	}
}

bool RemoveEmptyRulesImpl(std::vector<Rule>& rules)
{
	std::unordered_set<std::string> epsables;
	std::remove_reference<decltype(rules)>::type tmpRules;
	tmpRules.reserve(rules.size());

	for (const auto & [left, right] : rules)
	{
		if (const auto firstRight = right.front(); IsEmptyRule(firstRight) && (left != rules.front().left))
		{
			epsables.insert(left);
		}
		else
		{
			tmpRules.push_back({left, right});
		}
	}
	rules = std::move(tmpRules);

	for (const auto & [left, right] : rules)
	{
		if (std::any_of(right.cbegin(), right.cend(), [&epsables](const auto& symbol) {
				return epsables.find(symbol) != epsables.cend();
			}))
		{
			for (const auto & rule : Combine(right, epsables))
			{
				tmpRules.push_back({left, rule});
			}
		}
		else
		{
			tmpRules.push_back({left, right});
		}
	}

	for (auto & [left, right] : tmpRules)
	{
		if (right.empty())
		{
			right.push_back(NONTERMINAL_END_SEQUENCE);
		}
	}

	rules = std::move(tmpRules);
	return epsables.empty();
}

void RemoveEmptyRules(std::vector<Rule>& rules, std::unordered_set<std::string>& terminals)
{
	while (!RemoveEmptyRulesImpl(rules));

	terminals.erase(NONTERMINAL_END_SEQUENCE);
	terminals.insert(TERMINAL_END_SEQUENCE);
}

void AddEndTerminalToAxiom(std::vector<Rule>& rules)
{
	for (auto& [left, right] : rules)
	{
		if (const auto axiom = rules.front().left; left == axiom)
		{
			if (IsEmptyRule(right.front()))
			{
				right.clear();
			}
			right.push_back(TERMINAL_END_SEQUENCE);
		}
	}
}

std::vector<Rule> GetGrammar(std::istream& input)
{	
	auto [rules, nonTerminals, terminals] = GetRules(input);
	ValidateRules(rules, nonTerminals);
	ValidateGrammar(rules);
	
	MakeReplenished(rules, nonTerminals);
	RemoveEmptyRules(rules, terminals);
	AddEndTerminalToAxiom(rules);

	if constexpr (GeneratorSettings::USE_LEXER)
	{
		for (auto& [left, right] : rules)
		{
			std::transform(right.cbegin(), right.cend(), right.begin(), [](const auto& ch) {
				return IsNonTerminal(ch)
					? ch
					: (IsEndRule(ch) ? ch : LexemeTypeToString(ClassifyLexeme(ch)));
			});
		}
	}
	return rules;
}
