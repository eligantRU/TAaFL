#pragma once

#include <string>
#include <vector>
#include <set>

namespace Settings
{

constexpr auto USE_LEXER = false;
constexpr auto USE_OPTIMIZED_TABLE = true;

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
	else if constexpr (std::is_arithmetic<T>::value)
	{
		return std::to_string(arg);
	}
	else
	{
		return static_cast<std::string>(arg);
	}
};

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
