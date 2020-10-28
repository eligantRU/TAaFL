#pragma once

#include <string>
#include <vector>
#include <set>

namespace Settings
{

constexpr auto USE_LEXER = false;
constexpr auto USE_OPTIMIZED_TABLE = false;

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
	explicit ShiftReduceConflict(const std::string_view& msg, std::string ch)
		:std::domain_error(msg.data())
		,m_collisionChar(std::move(ch))
	{
	}

	std::string CollisionChar() const
	{
		return m_collisionChar;
	}

private:
	std::string m_collisionChar;
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
			return "S" + std::to_string(1 + std::distance(mainColumn->cbegin(), std::find(mainColumn->cbegin(), mainColumn->cend(), value)));
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

	operator std::string()
	{
		if constexpr (Settings::USE_OPTIMIZED_TABLE) // TODO: optimize it
		{
			return "R" + ToString(value);
		}
		else
		{
			return "R" + ToString(value);
		}
	}
};
