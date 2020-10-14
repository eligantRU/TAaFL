#include "GeneratorLR.h"

namespace
{

std::vector<std::string> GetUniqueCharacters(const std::vector<OutputDataGuideSets>& datas)
{
	std::set<std::string> result;
	for (const auto& [left, right, guide] : datas)
	{
		result.emplace(left);
		result.insert(right.cbegin(), right.cend());
	}
	std::vector<std::string> vecResult(result.cbegin(), result.cend());
	vecResult.erase(std::remove_if(vecResult.begin(), vecResult.end(), [&datas](const auto& ch) {
		return (ch == TERMINAL_END_SEQUENCE) || (ch == datas.front().nonterminal);
	}), vecResult.end());
	vecResult.insert(vecResult.begin(), datas.front().nonterminal);
	vecResult.insert(vecResult.end(), TERMINAL_END_SEQUENCE);
	return vecResult;
}

std::vector<std::vector<std::vector<std::string>>> MakeTable(size_t width, size_t height)
{
	std::vector<std::vector<std::vector<std::string>>> result;
	for (size_t i = 0; i < height; ++i)
	{
		std::vector<std::vector<std::string>> tmp(width);
		result.emplace_back(width);
	}
	return result;
}

}

GeneratorLR::GeneratorLR(const std::vector<OutputDataGuideSets>& datas)
	:m_datas(datas)
	,m_chars(GetUniqueCharacters(datas))
	,m_table(MakeTable(m_chars.size(), m_datas.size()))
{
	Generate();
}

void GeneratorLR::Generate()
{
	for (const auto& data : m_datas)
	{
		int a = 4;
	}
}

void GeneratorLR::Print(std::ostream& output)
{
	output << "Number" << TAB << "Char" << TAB;
	PrintInfoVector(output, m_chars, TAB);
	output << std::endl;
	for (size_t i = 0; i < m_table.size(); ++i)
	{
		output << i << TAB << (i ? "TODO" : "Init") << TAB;
		for (const auto& action : m_table[i])
		{
			PrintInfoVector(output, action, "+");
			output << "_"; // TODO:
		}
		output << std::endl;
	}
}
