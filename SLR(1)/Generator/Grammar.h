#pragma once

#include <unordered_set>
#include <algorithm>
#include <istream>

#include "../CommonLib/Rule.h"

using Grammar = std::vector<Rule>;
using NonTerminalsSet = std::unordered_set<std::string>;
using TerminalsSet = std::unordered_set<std::string>;

bool IsNonTerminal(const std::string_view& sv);

bool IsEmptyRule(const std::string_view& sv);

bool IsEndRule(const std::string_view& sv);

std::string GenerateNonTerminal();

std::tuple<Grammar, NonTerminalsSet, TerminalsSet> GetRules(std::istream& input);

void ValidateRules(const Grammar& rules, const NonTerminalsSet& nonTerminals);

void ValidateGrammar(const Grammar& rules);

bool IsReplenishedGrammar(const Grammar& rules);

std::tuple<Grammar, NonTerminalsSet, TerminalsSet> MakeReplenished(Grammar grammar, NonTerminalsSet nonTerminals, TerminalsSet terminals);

std::tuple<Grammar, TerminalsSet> RemoveEmptyRules(Grammar grammar, TerminalsSet terminals);

Grammar GetGrammar(std::istream& input);
