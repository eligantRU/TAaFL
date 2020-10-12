#include "GuideSets.h"

std::vector<std::vector<std::string>> Satan(const std::vector<std::string>& rule, const std::set<std::string>& epsables, const std::vector<std::vector<std::string>>& prevs = {}, size_t pos = 0)
{
	if (rule.empty())
	{
		throw std::exception("Rule cannot be empty");
	}
	if (pos == rule.size())
	{
		return prevs;
	}

	const auto curr = rule[pos];
	const auto isEpsilable = epsables.find(curr) != epsables.cend();
	
	using Bla = std::vector<std::vector<std::string>>;
	if (pos == 0)
	{
		return Satan(rule, epsables, isEpsilable ? Bla{ { curr }, {} } : Bla{ { curr } }, pos + 1);
	}
	else
	{
		std::vector<std::vector<std::string>> newPrevs = isEpsilable ? Bla{ prevs.cbegin(), prevs.cend() } : Bla{};
		for (auto prev : prevs)
		{
			prev.push_back(curr);
			newPrevs.push_back(prev);
		}
		return Satan(rule, epsables, newPrevs, pos + 1);
	}
}

void RemoveEmptyRules(std::vector<InputData>& inputDatas)
{
	std::set<std::string> epsables;
	std::vector<InputData> tmpInputDatas;
	tmpInputDatas.reserve(inputDatas.size());

	for (const auto & [left, right] : inputDatas)
	{
		if (const auto firstRight = right.front(); IsEmptyRule(firstRight))
		{
			epsables.insert(left);
		}
		else
		{
			tmpInputDatas.push_back({left, right});
		}
	}
	inputDatas = std::move(tmpInputDatas);

	for (const auto & [left, right] : inputDatas)
	{
		if (std::any_of(right.cbegin(), right.cend(), [&epsables](const auto& symbol) {
				return epsables.find(symbol) != epsables.cend();
			}))
		{
			for (const auto & rule : Satan(right, epsables))
			{
				tmpInputDatas.push_back({left, rule});
			}
		}
		else
		{
			tmpInputDatas.push_back({left, right});
		}
	}

	for (auto & [left, right] : tmpInputDatas)
	{
		if (right.empty())
		{
			if (left == tmpInputDatas.front().nonterminal)
			{
				right.push_back(TERMINAL_END_SEQUENCE);
			}
			else
			{
				std::cerr << " >> Empty non-axiom rule" << std::endl;
			}
		}
	}

	inputDatas = std::move(tmpInputDatas);
}

std::vector<OutputDataGuideSets> GetFormingGuideSets(std::istream& fileInput, std::vector<std::string>& characters, std::vector<TableData>& tableDataGuideSets)
{
	std::vector<InputData> inputDatas;
	std::vector<OutputDataGuideSets> outputDatas;

	std::vector<std::string> nonterminals;
	std::vector<std::string> terminals;

	FillingData(fileInput, inputDatas, nonterminals, terminals);
	RemoveEmptyRules(inputDatas);
	Forming(inputDatas, outputDatas, nonterminals, terminals);

	characters = GetCharacters(nonterminals, terminals);

	AddingGuideCharacters(outputDatas, tableDataGuideSets, characters, nonterminals);

	return outputDatas;
}

std::vector<std::string> GetCharacters(const std::vector<std::string>& nonterminals, const std::vector<std::string>& terminals)
{
	std::vector<std::string> characters;

	characters.insert(characters.end(), nonterminals.begin(), nonterminals.end());
	characters.insert(characters.end(), terminals.begin(), terminals.end());

	return characters;
}

void FillingData(std::istream& fileInput, std::vector<InputData>& inputDatas, std::vector<std::string>& nonterminals, std::vector<std::string>& terminals)
{
	std::string line;
	while (std::getline(fileInput, line))
	{
		std::istringstream iss(line);
		std::string str;

		InputData inputData;
		bool isTerminal = false;

		while (iss >> str)
		{
			if (str == DELIMITER)
			{
				isTerminal = true;
			}
			else if (isTerminal)
			{
				std::string newStr = str;

				if (!IsNonterminal(str))
				{
					bool isEndSequence = IsEmptyRule(str) || IsEndRule(str);

					if (!isEndSequence)
					{
						std::stringstream strm(str);

						Lexer lexer(strm);
						const auto [type, lexeme, lineNum, linePos] = lexer.GetLexeme();
						// TODO:
						// newStr = LexemeTypeToString(type);
						newStr = lexeme;
					}

					if (IsCheckUniqueness(terminals, newStr))
					{
						terminals.push_back(isEndSequence ? NONTERMINAL_END_SEQUENCE : newStr);
					}
				}

				inputData.terminals.push_back(newStr);
			}
			else
			{
				inputData.nonterminal = str;

				if (IsCheckUniqueness(nonterminals, str))
				{
					nonterminals.push_back(str);
				}
			}
		}
		inputDatas.push_back(inputData);
	}
}

void Forming(const std::vector<InputData>& inputDatas, std::vector<OutputDataGuideSets>& outputDatas, std::vector<std::string>& nonterminals, std::vector<std::string>& terminals)
{
	for (auto& nonterminal : nonterminals)
	{
		std::vector<InputData> temporaryVector;

		for (auto it = inputDatas.begin(); it != inputDatas.end(); ++it)
		{
			it = std::find_if(it, inputDatas.end(), [&](const InputData& data) { return data.nonterminal == nonterminal; });
			if (it == inputDatas.end())
			{
				break;
			}

			if (std::find_if(temporaryVector.begin(), temporaryVector.end(), [&](const InputData& data) { return data.terminals == (*it).terminals; }) == temporaryVector.end())
			{
				temporaryVector.push_back(*it);
			}
		}

		std::sort(temporaryVector.begin(), temporaryVector.end(), [](const InputData& a, const InputData& b) { return a.terminals.front() != b.terminals.front() && a.terminals.front() == a.nonterminal; });
		std::for_each(temporaryVector.begin(), temporaryVector.end(), [&](const InputData& data) { outputDatas.push_back({ data.nonterminal, data.terminals }); });
	}

	auto bla = false;
	for (const auto& outputData : outputDatas)
	{
		if (auto it = std::find(outputData.terminals.cbegin(), outputData.terminals.cend(), outputDatas.front().nonterminal);
			it != outputData.terminals.cend())
		{
			bla = true;
			break;
		}
	}
	if (bla)
	{
		std::string randomNonterminal;
		while (true)
		{
			randomNonterminal = "<" + GetRandomString() + ">";

			if (IsCheckUniqueness(nonterminals, randomNonterminal))
			{
				break;
			}
		}

		nonterminals.insert(nonterminals.begin(), randomNonterminal);
		outputDatas.insert(outputDatas.begin(), { randomNonterminal, std::vector<std::string>{ outputDatas.front().nonterminal } });
	}

	if (!IsEndRule(outputDatas.front().terminals.back()))
	{
		if (IsCheckUniqueness(terminals, NONTERMINAL_END_SEQUENCE))
		{
			terminals.push_back(NONTERMINAL_END_SEQUENCE);
		}

		outputDatas.front().terminals.push_back(TERMINAL_END_SEQUENCE);
	}
}

void AddingGuideCharacters(std::vector<OutputDataGuideSets>& outputDatas, std::vector<TableData>& tableDataGuideSets, const std::vector<std::string>& characters, const std::vector<std::string>& nonterminals)
{
	std::vector<PairStringVectorPair> transitions;
	std::vector<PairStringBool> charactersValue;

	std::for_each(characters.begin(), characters.end(), [&](const std::string str) { charactersValue.push_back(std::make_pair(str, false)); });
	std::for_each(nonterminals.begin(), nonterminals.end(), [&](const std::string str) { transitions.push_back(std::make_pair(str, charactersValue)); });

	BuildingFirstRelationship(outputDatas, transitions, charactersValue, tableDataGuideSets);
	BuildingFirstPlusRelationship(transitions);

	FormingTableDataGuideSets(tableDataGuideSets);

	for (auto& outputData : outputDatas)
	{
		size_t row = std::distance(transitions.begin(), GetIteratorFindIfVector(transitions, outputData.nonterminal));

		if (row < transitions.size())
		{
			std::vector<std::string> guideCharacters;
			std::string firstTerminal = outputData.terminals.front();

			for (auto& transitionSecond : transitions[row].second)
			{
				if (transitionSecond.second && !transitionSecond.first.empty())
				{
					if (outputData.nonterminal == transitionSecond.first)
					{
						guideCharacters.push_back(transitionSecond.first);
						transitionSecond.first = "";
						break;
					}

					if (outputData.nonterminal != firstTerminal && IsNonterminal(firstTerminal) || firstTerminal == transitionSecond.first || IsEmptyRule(firstTerminal))
					{
						guideCharacters.push_back(IsEmptyRule(transitionSecond.first) ? TERMINAL_END_SEQUENCE : transitionSecond.first);
						transitionSecond.first = "";
					}
				}
			}
			outputData.guideCharacters = guideCharacters;
		}
	}
}

void FormingTableDataGuideSets(std::vector<TableData>& tableDataGuideSets)
{
	std::vector<TableData> copyTables;
	std::copy(tableDataGuideSets.begin(), tableDataGuideSets.end(), std::back_inserter(copyTables));

	for (auto it = copyTables.rbegin(); it != copyTables.rend(); ++it)
	{
		it = std::find_if(it, copyTables.rend(), [&](const TableData& data) { return IsNonterminal(data.character); });

		if (it == copyTables.rend())
		{
			break;
		}

		std::vector<TableData> vec;
		std::copy(tableDataGuideSets.begin(), tableDataGuideSets.end(), std::back_inserter(vec));

		for (auto it2 = vec.begin(); it2 != vec.end(); ++it2)
		{
			it2 = std::find_if(it2, vec.end(), [&](const TableData& data) { return data.nonterminal == (*it).character && data.nonterminal != (*it).nonterminal && data.character != (*it).character; });

			if (it2 == vec.end())
			{
				break;
			}

			tableDataGuideSets.push_back({ (*it).nonterminal, (*it2).character, (*it2).row, (*it2).position });
		}
	}
}

void BuildingFirstRelationship(std::vector<OutputDataGuideSets>& outputData, std::vector<PairStringVectorPair>& transitions, std::vector<PairStringBool>& characters, std::vector<TableData>& tableDataGuideSets)
{
	for (size_t i = 0; i < outputData.size(); ++i)
	{
		size_t row = std::distance(transitions.begin(), GetIteratorFindIfVector(transitions, outputData[i].nonterminal));
		size_t column = std::distance(characters.begin(), GetIteratorFindIfVector(characters, outputData[i].terminals.front()));

		if (row < transitions.size() && column < characters.size())
		{
			if (IsEmptyRule(outputData[i].terminals.front()))
			{
				SearchStartingTerminalsEmptyRules(outputData, outputData[i].nonterminal, outputData[i].nonterminal, transitions, characters, tableDataGuideSets);
			}
			else
			{
				tableDataGuideSets.push_back({ outputData[i].nonterminal, characters[column].first, i });
				transitions[row].second[column].second = true;
			}
		}
	}
}

void SearchStartingTerminalsEmptyRules(std::vector<OutputDataGuideSets>& outputDatas, const std::string parentNonterminal, const std::string nonterminal,
	std::vector<PairStringVectorPair>& transitions, std::vector<PairStringBool>& characters, std::vector<TableData>& tableDataGuideSets)
{
	for (size_t i = 0; i < outputDatas.size(); ++i)
	{
		auto outputData = outputDatas[i];
		auto it1 = std::find_if(outputData.terminals.begin(), outputData.terminals.end(), [&](const std::string& str) { return str == nonterminal; });

		if (it1 != outputData.terminals.end())
		{
			size_t distance = std::distance(outputData.terminals.begin(), it1);
			size_t size = outputData.terminals.size() - 1;
			std::string terminal = distance <= size ? (distance < size ? outputData.terminals[distance + 1] : outputData.terminals.back()) : NONTERMINAL_END_SEQUENCE;

			size_t row = std::distance(transitions.begin(), GetIteratorFindIfVector(transitions, parentNonterminal));
			size_t column = std::distance(characters.begin(), GetIteratorFindIfVector(characters, IsEndRule(terminal) ? NONTERMINAL_END_SEQUENCE : terminal));

			if (row < transitions.size() && column < characters.size() && terminal != parentNonterminal)
			{
				TableData tableData{ parentNonterminal, transitions[row].second[column].first, i, GetDistanceVector(outputData.terminals, terminal) };

				if (IsCheckTableDataUniqueness(tableDataGuideSets, tableData))
				{
					tableDataGuideSets.push_back(tableData);
				}

				transitions[row].second[column].second = true;
			}

			if (nonterminal == parentNonterminal && outputData.nonterminal != nonterminal)
			{
				for (auto& data : outputDatas)
				{
					auto it2 = std::find_if(data.terminals.begin(), data.terminals.end(), [&](const std::string& str) { return str == outputData.nonterminal; });

					if (it2 != data.terminals.end())
					{
						SearchStartingTerminalsEmptyRules(outputDatas, parentNonterminal, (*it2), transitions, characters, tableDataGuideSets);
					}
				}
			}
		}
	}
}

void BuildingFirstPlusRelationship(std::vector<PairStringVectorPair>& transitions)
{
	for (auto it = transitions.rbegin(); it != transitions.rend(); ++it)
	{
		for (size_t j = 0; j < transitions.size(); ++j)
		{
			if ((*it).second[j].second)
			{
				size_t row = std::distance(transitions.begin(), GetIteratorFindIfVector(transitions, (*it).second[j].first));

				for (size_t k = 0; k < transitions[row].second.size(); ++k)
				{
					if (transitions[row].second[k].second)
					{
						(*it).second[k].second = true;
					}
				}
			}
		}
	}
}

void PrintResultGuideSets(std::ofstream& fileOutput, const std::vector<OutputDataGuideSets>& outputDatas)
{
	for (const auto& outputData : outputDatas)
	{
		fileOutput << outputData.nonterminal << SPACE << DELIMITER << SPACE;
		PrintInfoVector(fileOutput, outputData.terminals, SPACE);
		fileOutput << SPACE << "/" << SPACE;
		PrintInfoVector(fileOutput, outputData.guideCharacters, SPACE);
		fileOutput << std::endl;
	}
}