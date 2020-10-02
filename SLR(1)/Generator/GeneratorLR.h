#pragma once
#include "Utils.h"
#include <stack>

class GeneratorLR
{
public:
	GeneratorLR(std::ofstream& fileOutput, const std::vector<OutputDataGuideSets>& inputDatas, const std::vector<std::string>& characters, const std::vector<TableData>& tableDataGuideSets);

	void Generate();
	void PrintResult();

private:
	std::ostream& m_output;

	std::vector<OutputDataGuideSets> m_inputDatas;
	std::vector<OutputData> m_outputDatas;
	std::vector<OutputDataGenerator> m_outputGeneratorDatas;

	std::vector<std::string> m_characters;
	std::vector<TableData> m_tableDataGuideSets;
	std::vector<TableData> m_rightSides;
	std::vector<TableData> m_states;
	std::stack<std::vector<TableData>> m_stack;

	void FormingTransitions(const TableData& tableData, std::vector<Transition>& transitions);

	std::vector<TableData>::iterator FindEmptyRuleInRightSides(const std::string str);

	void WriteTransitions(const TableData& tableData, const std::string str, std::vector<Transition>& transitions, const TableData& firstData);

	void AddShift(const TableData& tableData, std::vector<Transition>& transitions);
	void AddRollUp(const TableData& tableData, const std::string str, std::vector<Transition>& transitions);
	void AddEvent(const TableData& tableData, const std::string str, const StateGenerator state, std::vector<Transition>& transitions);

	void AddRollUpEmptyRule(const TableData& tableData, const std::string str, std::vector<Transition>& transitions);

	void FindEvent(const TableData& tableData, const std::string parentNonterminal, const std::string nonterminal, const StateGenerator state, std::vector<Transition>& transitions);

	void AddingStackItems(std::vector<Transition>& transitions);

	void ConvertingTable();

	std::vector<TableData> GetTableDatasFindGuideSets(const std::vector<TableData>& vec, const std::string str);

	std::vector<TableData> GetRightSides();
};