#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <set>

namespace GeneratorSettings
{

constexpr auto USE_LEXER = true;
constexpr auto USE_OPTIMIZED_TABLE = true;
constexpr auto USE_PRETTY_TABLE = true;

}

constexpr auto SPACE = " ";
constexpr auto SEPARATOR = "=>";
constexpr auto NONTERMINAL_END_SEQUENCE = "e";
constexpr auto TERMINAL_END_SEQUENCE = "$";
constexpr auto NONTERMINAL_GEN_PREFIX = "_gen";

struct Rule
{
	std::string left;
	std::vector<std::string> right;
};

using Grammar = std::set<Rule>; // TODO:

bool operator==(const Rule& lhs, const Rule& rhs)
{
	return (lhs.left == rhs.left) && (lhs.right == rhs.right);
}

bool operator!=(const Rule& lhs, const Rule& rhs)
{
	return !(lhs == rhs);
}

class ShiftReduceConflict
	:public std::domain_error
{
public:
	explicit ShiftReduceConflict(size_t ruleNum)
		:std::domain_error("Shift/Reduce conflict - not a SLR(1) grammar")
		,m_ruleNum(ruleNum)
	{
	}

	size_t ConflictRuleNum() const
	{
		return m_ruleNum;
	}

private:
	size_t m_ruleNum;
};

template <class T>
constexpr std::string ToString(T arg)
{
	if constexpr (std::is_same<T, std::string>::value)
	{
		return arg;
	}
	else if constexpr (std::is_same<T, char>::value)
	{
		return std::string(1, arg);
	}
	else if constexpr (std::is_same<T, std::string_view>::value)
	{
		return std::string(arg.data());
	}
	else if constexpr (std::is_arithmetic<T>::value)
	{
		return std::to_string(arg);
	}
	else
	{
		return static_cast<std::string>(arg);
	}
};

std::vector<std::string> Tokenize(const std::string& str, char separator = ' ')
{
	std::vector<std::string> result;
	std::stringstream stream(str);
	std::string tmp;
	while (std::getline(stream, tmp, separator))
	{
		if (!tmp.empty())
		{
			result.push_back(tmp);
		}
	}
	return result;
}
