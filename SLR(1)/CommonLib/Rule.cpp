#include "Rule.h"

bool operator<(const Rule& lhs, const Rule& rhs) noexcept
{
	if (lhs.left == rhs.left)
	{
		return lhs.right < rhs.right;
	}
	return lhs.left < rhs.left;
}

bool operator==(const Rule& lhs, const Rule& rhs) noexcept
{
	return (lhs.left == rhs.left) && (lhs.right == rhs.right);
}

bool operator!=(const Rule& lhs, const Rule& rhs) noexcept
{
	return !(lhs == rhs);
}
