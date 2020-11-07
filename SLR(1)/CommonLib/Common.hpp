#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <set>

constexpr auto SPACE = " ";
constexpr auto SEPARATOR = "=>";
constexpr auto NONTERMINAL_END_SEQUENCE = "e";
constexpr auto TERMINAL_END_SEQUENCE = "$";
constexpr auto NONTERMINAL_GEN_PREFIX = "_gen";

template <class T>
T Uniqify(const T & c)
{
	std::set bla(c.cbegin(), c.cend());
	return { bla.cbegin(), bla.cend() };
}

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

std::vector<std::string> Tokenize(const std::string& str, char separator = ' ');
