#include "Helper.h"

std::string GetString(std::istringstream& iss)
{
	std::string str;
	iss >> str;
	return str;
}

size_t ParseNumber(const std::string str)
{
	try
	{
		return std::stoi(str);
	}
	catch (const std::exception&)
	{
		throw std::invalid_argument("Invalid argument: " + str);
	}
}

Symbol ParseSymbol(std::string str)
{
	StateSymbol state;
	size_t number;

	if (str == "-")
	{
		state = StateSymbol::None;
		number = 0;
	}
	else if (str == "Ok")
	{
		state = StateSymbol::Ok;
		number = 0;
	}
	else
	{
		state = ParseState(str[0]);
		str.erase(0, 1);
		number = ParseNumber(str);
	}

	Symbol symbol = {
		state,
		number
	};

	return symbol;
}

StateSymbol ParseState(char fCh)
{
	if (fCh == 'S')
	{
		return StateSymbol::Shift;
	}
	else if (fCh == 'R')
	{
		return StateSymbol::Convolution;
	}
	else
	{
		throw std::exception("Error, when trying parse state from char " + fCh);
	}
}

bool IsNonterminal(const std::string str)
{
	return !str.empty() && str.front() == '<' && str.back() == '>';
}
