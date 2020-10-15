#pragma once

#include "Utils.h"

class GeneratorLR
{
public:
	GeneratorLR(const std::vector<OutputDataGuideSets>& datas);

	void Generate();
	void Print(std::ostream& output) const;

private:
	std::map<std::string, std::set<std::variant<size_t, std::pair<size_t, size_t>>>> ColdStart() const;
	void TransitionsToTable(const std::map<std::string, std::set<std::variant<size_t, std::pair<size_t, size_t>>>>& transitions);
	std::set<std::set<std::pair<std::string, std::pair<size_t, size_t>>>> GetNextToProcess(const std::map<std::string, std::set<std::variant<size_t, std::pair<size_t, size_t>>>>& transitions) const;

	std::vector<OutputDataGuideSets> m_datas;

	std::vector<std::string> m_chars;
	std::vector<std::vector<std::set<std::string>>> m_table;
	std::vector<std::set<std::pair<std::string, std::pair<size_t, size_t>>>> m_mainColumn;
};
