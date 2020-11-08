#include "Common.hpp"

std::vector<std::string> Tokenize(const std::string& str, char separator)
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
