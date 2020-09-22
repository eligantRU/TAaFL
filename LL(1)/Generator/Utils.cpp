#include "Utils.h"

bool IsCheckUniqueness(const std::vector<std::string>& vec, const std::string& str)
{
	return std::find(vec.begin(), vec.end(), str) == vec.end();
}

bool IsNonterminal(const std::string& str)
{
	return !str.empty() && (str.front() == '<') && (str.back() == '>');
}

bool IsEmptyRule(const std::string& str)
{
	return (str == NONTERMINAL_END_SEQUENCE);
}

bool IsEndRule(const std::string& str)
{
	return (str == TERMINAL_END_SEQUENCE);
}

size_t GetRandomNumber(size_t min, size_t max)
{
	return (std::rand() % max) + min;
}

std::string SubstrNonterminal(std::string str)
{
	return str.substr(1, str.length() - 2);
}

std::string GetRandomString()
{
#ifdef _DEBUG
	static size_t callCounter = 0;
	return "_gen" + std::to_string(callCounter++);
#endif
	const std::string characters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const size_t length = GetRandomNumber(1, 8);

	std::string str;
	str.reserve(length);
	for (size_t i = 0; i < length; ++i)
	{
		str += characters[std::rand() % characters.size()];
	}
	return str;
}

void PrintInfoVector(std::ostream& output, const std::vector<std::string>& vec, std::string str)
{
	if (!vec.empty())
	{
		std::copy(vec.begin(), vec.end() - 1, std::ostream_iterator<std::string>(output, str.c_str()));
		output << vec.back();
	}
}
