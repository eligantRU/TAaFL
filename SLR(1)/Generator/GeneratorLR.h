#pragma once

#include "Utils.h"

class GeneratorLR
{
public:
	GeneratorLR(const std::vector<OutputDataGuideSets>& datas);

	void Generate();
	void Print(std::ostream& output);

private:
	std::vector<OutputDataGuideSets> m_datas;

	std::vector<std::string> m_chars;
	std::vector<std::vector<std::vector<std::string>>> m_table;
};
