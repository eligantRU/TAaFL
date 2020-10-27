#pragma once

#include <string>
#include <vector>
#include <set>

struct Rule
{
	std::string left;
	std::vector<std::string> right;
};

constexpr auto SPACE = " ";
constexpr auto SEPARATOR = "=>";
constexpr auto NONTERMINAL_END_SEQUENCE = "e";
constexpr auto TERMINAL_END_SEQUENCE = "$";
constexpr auto NONTERMINAL_GEN_PREFIX = "_gen";

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

struct Shift
{
	std::set<std::pair<size_t, size_t>> value;

	operator std::string()
	{
		if (value.empty())
		{
			return "-";
		}

		std::string res = "S<";
		for (size_t i = 0; i < value.size(); ++i)
		{
			auto it = value.cbegin();
			std::advance(it, i);
			const auto& [row, col] = *it;
			res += (i ? "|" : "") + ToString(row) + "," + ToString(col);
		}
		return res + ">";
	}
};

struct Reduce
{
	size_t value;

	operator std::string()
	{
		return "R" + ToString(value);
	}
};
