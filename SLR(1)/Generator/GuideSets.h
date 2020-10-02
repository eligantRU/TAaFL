#pragma once

#include "../../Lexer/Lexer.hpp"
#include "Utils.h"

std::vector<OutputDataGuideSets> GetFormingGuideSets(std::istream& fileInput, std::vector<std::string>& characters, std::vector<TableData>& tableDataGuideSets);

void FillingData(std::istream& fileInput, std::vector<InputData>& inputDatas, std::vector<std::string>& nonterminals, std::vector<std::string>& terminals);
void Forming(const std::vector<InputData>& inputDatas, std::vector<OutputDataGuideSets>& outputDatas, std::vector<std::string>& nonterminals, std::vector<std::string>& terminals);

std::vector<std::string> GetCharacters(const std::vector<std::string>& nonterminals, const std::vector<std::string>& terminals);

void FormingTableDataGuideSets(std::vector<TableData>& tableDataGuideSets);

void AddingGuideCharacters(std::vector<OutputDataGuideSets>& outputDatas, std::vector<TableData>& tableDataGuideSets, const std::vector<std::string>& characters, const std::vector<std::string>& nonterminals);
void BuildingFirstRelationship(std::vector<OutputDataGuideSets>& outputDatas, std::vector<PairStringVectorPair>& transitions, std::vector<PairStringBool>& charactersValue, std::vector<TableData>& tableDataGuideSets);
void BuildingFirstPlusRelationship(std::vector<PairStringVectorPair>& transitions);
void SearchStartingTerminalsEmptyRules(std::vector<OutputDataGuideSets>& outputDatas, const std::string parentNonterminal,
	const std::string nonterminal, std::vector<PairStringVectorPair>& transitions, std::vector<PairStringBool>& charactersValue, std::vector<TableData>& tableDataGuideSets);

void PrintResultGuideSets(std::ofstream& fileOutput, const std::vector<OutputDataGuideSets>& outputDatas);