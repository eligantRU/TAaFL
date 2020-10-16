#pragma once

#include "Utils.h"

class GeneratorSLR
{
public:
	GeneratorSLR(const std::vector<OutputDataGuideSets>& datas);

	void Generate();
	void Print(std::ostream& output) const;

private:
	std::map<std::string, std::variant<std::set<std::pair<size_t, size_t>>, size_t>> ColdStart() const;
	void TransitionsToTable(const std::map<std::string, std::variant<std::set<std::pair<size_t, size_t>>, size_t>>& transitions);
	std::set<std::set<std::pair<size_t, size_t>>> GetNextToProcess(const std::map<std::string, std::variant<std::set<std::pair<size_t, size_t>>, size_t>>& transitions) const;

	std::vector<OutputDataGuideSets> m_datas;

	std::vector<std::string> m_chars;
	std::vector<std::vector<std::variant<std::set<std::pair<size_t, size_t>>, size_t>>> m_table;
	std::vector<std::set<std::pair<size_t, size_t>>> m_mainColumn;
};
