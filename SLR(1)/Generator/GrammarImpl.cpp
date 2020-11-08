#include "GrammarImpl.h"

#include "../CommonLib/Common.hpp"

namespace
{

std::vector<std::vector<std::string>> Combine(const std::vector<std::string>& rule, const NonTerminalsSet& epsables, const std::vector<std::vector<std::string>>& prevs = {}, size_t pos = 0)
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

bool RemoveEmptyRulesImpl(Grammar& grammar)
{
	NonTerminalsSet epsables;
	std::remove_reference<decltype(grammar)>::type tmpGrammar;
	tmpGrammar.reserve(grammar.size());

	for (const auto & [left, right] : grammar)
	{
		if (const auto firstRight = right.front(); IsEmptyRule(firstRight) && (left != grammar.front().left))
		{
			epsables.insert(left);
		}
		else
		{
			tmpGrammar.push_back({left, right});
		}
	}
	grammar = std::move(tmpGrammar);

	for (const auto & [left, right] : grammar)
	{
		if (std::any_of(right.cbegin(), right.cend(), [&epsables](const auto& symbol) {
				return epsables.find(symbol) != epsables.cend();
			}))
		{
			for (const auto & rule : Combine(right, epsables))
			{
				tmpGrammar.push_back({left, rule});
			}
		}
		else
		{
			tmpGrammar.push_back({left, right});
		}
	}

	for (auto & [left, right] : tmpGrammar)
	{
		if (right.empty())
		{
			right.push_back(NONTERMINAL_END_SEQUENCE);
		}
	}

	grammar = std::move(tmpGrammar);
	return epsables.empty();
}

std::tuple<Grammar, TerminalsSet> AddEndTerminalToAxiom(Grammar grammar, TerminalsSet terminals)
{
	for (auto& [left, right] : grammar)
	{
		if (const auto axiom = grammar.front().left; left == axiom)
		{
			if (IsEmptyRule(right.front()))
			{
				right.clear();
			}
			right.push_back(TERMINAL_END_SEQUENCE);
		}
	}
	terminals.insert(TERMINAL_END_SEQUENCE);

	return { grammar, terminals };
}

}

namespace Impl
{

std::tuple<Grammar, NonTerminalsSet, TerminalsSet> GetRules(std::istream& input)
{
	Grammar grammar;
	NonTerminalsSet nonTerminals;
	TerminalsSet terminals;

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
		grammar.push_back({ tokens.front(), { tokens.cbegin() + 2, tokens.cend() } });
		nonTerminals.insert(tokens[0]);
		std::for_each(tokens.cbegin() + 2, tokens.cend(), [&terminals](const std::string_view& sv) {
			if (!IsNonTerminal(sv) && !IsEmptyRule(sv) && !IsEndRule(sv))
			{
				terminals.emplace(sv);
			}
		});
	}
	return std::make_tuple(grammar, nonTerminals, terminals);
}

void ValidateRules(const Grammar& grammar, const NonTerminalsSet& nonTerminals)
{
	for (const auto& [left, right]: grammar)
	{
		if (IsEmptyRule(right.front()) && (right.size() > 1))
		{
			throw std::logic_error("Empty-rule cannot contains another symbols");
		}

		if (left.substr(1, strlen(NONTERMINAL_GEN_PREFIX)) == std::string(NONTERMINAL_GEN_PREFIX))
		{
			throw std::logic_error(std::string(NONTERMINAL_GEN_PREFIX) + "-prefix cannot be used in non-terminals' names");
		}

		for (const auto& el : right)
		{
			if (IsNonTerminal(el))
			{
				if (!nonTerminals.count(el))
				{
					throw std::logic_error("Unknown non-terminal " + el);
				}
				if (el.substr(1, strlen(NONTERMINAL_GEN_PREFIX)) == std::string(NONTERMINAL_GEN_PREFIX))
				{
					throw std::logic_error(std::string(NONTERMINAL_GEN_PREFIX) + "-prefix cannot be used in non-terminals' names");
				}
			}
			if (el == TERMINAL_END_SEQUENCE)
			{
				throw std::logic_error("Right-side of the rule cannot contains end-rule terminal (in raw input)");
			}
		}
	}
}

void ValidateGrammar(const Grammar& grammar)
{
	if (grammar.empty())
	{
		throw std::logic_error("Empty grammar");
	}
}

bool IsReplenishedGrammar(const Grammar& grammar)
{
	for (const auto& rule : grammar)
	{
		if (const auto it = std::find(rule.right.cbegin(), rule.right.cend(), grammar.front().left);
			it != rule.right.cend())
		{
			return false;
		}
	}
	return true;
}

std::tuple<Grammar, NonTerminalsSet, TerminalsSet> MakeReplenished(Grammar grammar, NonTerminalsSet nonTerminals, TerminalsSet terminals)
{
	if (!IsReplenishedGrammar(grammar))
	{
		const auto randomNonterminal = GenerateNonTerminal();
		nonTerminals.insert(randomNonterminal);
		grammar.insert(grammar.begin(), { randomNonterminal, std::vector<std::string>{ grammar.front().left } });
	}
	std::tie(grammar, terminals) = AddEndTerminalToAxiom(grammar, terminals);
	return { grammar, nonTerminals, terminals };
}

std::tuple<Grammar, TerminalsSet> RemoveEmptyRules(Grammar grammar, TerminalsSet terminals)
{
	for (const auto& [left, right] : grammar)
	{
		if ((left == grammar.front().left) && (right.back() != TERMINAL_END_SEQUENCE))
		{
			throw std::logic_error("Rule should be ended by end-rule terminal");
		}
	}

	while (!RemoveEmptyRulesImpl(grammar));
	terminals.erase(NONTERMINAL_END_SEQUENCE);

	return { grammar, terminals };
}

}
