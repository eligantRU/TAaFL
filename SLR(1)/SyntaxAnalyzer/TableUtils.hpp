#pragma once

#include <optional>
#include <variant>
#include <regex>

#include "../CommonLib/Common.hpp"
#include "../CommonLib/Table.hpp"

constexpr auto REDUCE_REGEX = "<(\\w+)>\\|(\\d+)";

struct Shift
{
	size_t pointer;

	operator std::string()
	{
		return ToString(pointer);
	}
};

struct Reduce
{
	std::string ch;
	size_t len;

	operator std::string()
	{
		return ch + "|" + ToString(len);
	}
};

bool IsNumber(const std::string_view& sv)
{
	const std::string str = sv.data();
	return std::all_of(str.cbegin(), str.cend(), ::isdigit);
}

bool IsShift(const std::string_view& sv)
{
	return IsNumber(sv);
}

bool IsReduce(const std::string_view& sv)
{
	return std::regex_match(sv.data(), std::regex(REDUCE_REGEX));
}

bool IsEmpty(const std::string_view& sv)
{
	return sv == ToString(Table<>::EMPTY_CELL);
}

Shift ToShift(const std::string_view& sv)
{
	if (!IsShift(sv))
	{
		throw std::runtime_error(std::string("Not a shift-cell: ") + sv.data());
	}
	return { (size_t)std::stoi(ToString(sv)) };
}

Reduce ToReduce(const std::string_view& sv)
{
	if (!IsReduce(sv))
	{
		throw std::runtime_error(std::string("Not a reduce-cell: ") + sv.data());
	}
	std::cmatch match;
	std::regex_match(sv.data(), match, std::regex(REDUCE_REGEX));
	return { match[1], (size_t)std::stoi(match[2]) };
}

std::tuple<std::vector<std::string>, Table<std::optional<std::variant<Shift, Reduce>>>> GetTable(std::istream& inputTable)
{
	std::string line;
	std::getline(inputTable, line);
	const auto legend = Tokenize(line);
	return {
		legend,
		Table<std::optional<std::variant<Shift, Reduce>>>{inputTable, legend, [](const std::string_view& sv) -> std::optional<std::variant<Shift, Reduce>> {		
			if (IsShift(sv))
			{
				return ToShift(sv);
			}
			else if (IsReduce(sv))
			{
				return ToReduce(sv);
			}
			else if (IsEmpty(sv))
			{
				return std::nullopt;
			}
			throw std::domain_error(std::string("Unexpected cell value: ") + sv.data());
		}}
	};
}
