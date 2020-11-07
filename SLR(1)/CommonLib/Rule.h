#pragma once

#include <string>
#include <vector>

struct Rule
{
	std::string left;
	std::vector<std::string> right;
};

bool operator<(const Rule& lhs, const Rule& rhs) noexcept;

bool operator==(const Rule& lhs, const Rule& rhs) noexcept;

bool operator!=(const Rule& lhs, const Rule& rhs) noexcept;
