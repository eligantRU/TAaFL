#include "Grammar.h"

#include "../CommonLib/Common.hpp"

#include "GrammarImpl.h"

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

Grammar GetGrammar(std::istream& input)
{
	auto [grammar, nonTerminals, terminals] = Impl::GetRules(input);
	Impl::ValidateRules(grammar, nonTerminals);
	Impl::ValidateGrammar(grammar);

	std::tie(grammar, nonTerminals, terminals) = Impl::MakeReplenished(grammar, nonTerminals, terminals);
	std::tie(grammar, terminals) = Impl::RemoveEmptyRules(grammar, terminals);
	return grammar;
}
