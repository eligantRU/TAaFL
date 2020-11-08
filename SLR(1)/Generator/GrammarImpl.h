#pragma once

#include "Grammar.h"

namespace Impl
{

std::tuple<Grammar, NonTerminalsSet, TerminalsSet> GetRules(std::istream& input);

void ValidateRules(const Grammar& rules, const NonTerminalsSet& nonTerminals);

void ValidateGrammar(const Grammar& rules);

bool IsReplenishedGrammar(const Grammar& rules);

std::tuple<Grammar, NonTerminalsSet, TerminalsSet> MakeReplenished(Grammar grammar, NonTerminalsSet nonTerminals, TerminalsSet terminals);

std::tuple<Grammar, TerminalsSet> RemoveEmptyRules(Grammar grammar, TerminalsSet terminals);

}
