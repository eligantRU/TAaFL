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

Grammar GetGrammar(std::istream& input);
