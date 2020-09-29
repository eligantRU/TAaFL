#include "../../Lexer/Lexer.hpp"
#include "GuideSets.h"
#include "Utils.h"

namespace
{

std::vector<PairStringBool>::const_iterator GetIteratorFindIfVector(const std::vector<PairStringBool>& vec, std::string str)
{
	return std::find_if(vec.cbegin(), vec.cend(), [&](const PairStringBool& data) {
		return data.first == str;
	});
}

std::vector<PairStringVectorPair>::const_iterator GetIteratorFindIfVector(const std::vector<PairStringVectorPair>& vec, std::string str)
{
	return std::find_if(vec.cbegin(), vec.cend(), [&](const PairStringVectorPair& data) {
		return data.first == str;
	});
}

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
						newStr = lexeme; // TODO: LexemeTypeToString(type);
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

void ActionsRightSide(const std::vector<InputData>& inputDatas, std::vector<OutputDataGuideSets>& outputDatas,
	std::vector<std::string>& nonterminals, std::vector<std::string>& terminals,
	std::string nonterminal, const std::vector<InputData>& temporaryVector, bool isBlankCharacter)
{
	bool isLeftRecursion = false;
	bool isFactorization = false;

	for (size_t i = 0; i < temporaryVector.size(); ++i)
	{
		if (i < temporaryVector.size() - 1)
		{
			std::vector<std::string> first = temporaryVector.front().terminals;
			std::vector<std::string> second = temporaryVector[i + 1].terminals;

			isFactorization = std::all_of(first.begin(), first.end(), [&](const std::string str) {
				return std::find(second.begin(), second.end(), str) != second.end();
			});
		}

		if (temporaryVector[i].terminals.front() == nonterminal)
		{
			isLeftRecursion = true;
		}
	}

	std::string randomNonterminal;

	while (true) // TODO: -> GetRandomString
	{
		randomNonterminal = "<" + GetRandomString() + ">";

		if (IsCheckUniqueness(nonterminals, randomNonterminal))
		{
			break;
		}
	}

	if (isLeftRecursion || isFactorization)
	{
		nonterminals.push_back(randomNonterminal);
	}
	else if (temporaryVector.size() == 1)
	{
		size_t row = std::distance(inputDatas.begin(), std::find_if(inputDatas.begin(), inputDatas.end(), [&](const InputData& data) { return data.nonterminal == nonterminal; }));
		outputDatas.push_back({ inputDatas[row].nonterminal, inputDatas[row].terminals });
		return;
	}

	for (size_t i = 0; i < temporaryVector.size(); ++i)
	{
		InputData data = temporaryVector[i];
		OutputDataGuideSets out;

		if (isLeftRecursion)
		{
			data.terminals.push_back(randomNonterminal);

			if (isBlankCharacter && (i == 0))
			{
				outputDatas.push_back({ nonterminal, std::vector<std::string>{ randomNonterminal } });
			}

			if (data.terminals.front() == nonterminal)
			{
				out.nonterminal = randomNonterminal;
				out.terminals = std::vector<std::string>(data.terminals.begin() + 1, data.terminals.end());
			}
			else if (!isBlankCharacter)
			{
				out.nonterminal = nonterminal;
				out.terminals = data.terminals;
			}
		}
		else if (isFactorization)
		{
			if (i == 0)
			{
				data.terminals.push_back(randomNonterminal);
				out.nonterminal = nonterminal;
				out.terminals = data.terminals;
			}
			else
			{
				out.nonterminal = randomNonterminal;
				out.terminals = std::vector<std::string>(data.terminals.begin() + temporaryVector.front().terminals.size(), data.terminals.end());
			}
		}
		else
		{
			out.nonterminal = nonterminal;
			out.terminals = data.terminals;
		}

		if (!out.nonterminal.empty())
		{
			outputDatas.push_back(out);
		}
	}

	if (isLeftRecursion || isFactorization)
	{
		if (IsCheckUniqueness(terminals, NONTERMINAL_END_SEQUENCE))
		{
			terminals.push_back(NONTERMINAL_END_SEQUENCE);
		}

		outputDatas.push_back({ randomNonterminal, std::vector<std::string>{ NONTERMINAL_END_SEQUENCE } });
	}
}

void Forming(const std::vector<InputData>& inputDatas, std::vector<OutputDataGuideSets>& outputDatas, std::vector<std::string>& nonterminals, std::vector<std::string>& terminals)
{
	std::vector<std::string> copyNonterminals;
	std::copy(nonterminals.begin(), nonterminals.end(), std::back_inserter(copyNonterminals));
	for (auto& nonterminal : copyNonterminals)
	{
		std::vector<InputData> temporaryVector;
		bool isBlankCharacter = false;

		for (auto it = inputDatas.begin(); it != inputDatas.end(); ++it)
		{
			it = std::find_if(it, inputDatas.end(), [&](const InputData& data) { return data.nonterminal == nonterminal; });
			if (it == inputDatas.end())
			{
				break;
			}

			if (std::find_if(temporaryVector.begin(), temporaryVector.end(), [&](const InputData& data) {
					return data.terminals == (*it).terminals;
				}) == temporaryVector.end())
			{
				temporaryVector.push_back(*it);
			}
		}

		std::sort(temporaryVector.begin(), temporaryVector.end(), [](const InputData& a, const InputData& b) { return a.terminals.size() < b.terminals.size(); });
		auto it = std::find_if(temporaryVector.begin(), temporaryVector.end(), [&](const InputData& data) { return IsEmptyRule(data.terminals.front()); });

		if (it != temporaryVector.end())
		{
			isBlankCharacter = true;
			std::rotate(temporaryVector.begin(), it + 1, temporaryVector.end());
		}

		ActionsRightSide(inputDatas, outputDatas, nonterminals, terminals, nonterminal, temporaryVector, isBlankCharacter);
	}

	for (auto it = outputDatas.begin(); it != outputDatas.end(); ++it)
	{
		it = std::find_if(it, outputDatas.end(), [&](const OutputDataGuideSets& data) {
			return data.nonterminal == outputDatas.front().nonterminal;
		});
		if (it == outputDatas.end())
		{
			break;
		}

		if (!IsEndRule((*it).terminals.back()))
		{
			(*it).terminals.push_back(TERMINAL_END_SEQUENCE);
		}
	}
}

void SearchStartingTerminalsEmptyRules(const std::vector<OutputDataGuideSets>& outputDatas,
	std::string parentNonterminal, std::string nonterminal,
	std::vector<PairStringVectorPair>& transitions, const std::vector<PairStringBool>& characters)
{
	for (const auto& outputData : outputDatas)
	{
		if (auto it1 = std::find_if(outputData.terminals.begin(), outputData.terminals.end(), [&](const std::string& str) {
				return str == nonterminal;
			}); it1 != outputData.terminals.end())
		{
			size_t distance = std::distance(outputData.terminals.begin(), it1);
			size_t size = outputData.terminals.size() - 1;
			std::string terminal = (distance <= size) ? (distance < size ? outputData.terminals[distance + 1] : outputData.terminals.back()) : NONTERMINAL_END_SEQUENCE;

			size_t row = std::distance(transitions.cbegin(), GetIteratorFindIfVector(transitions, parentNonterminal));
			size_t column = std::distance(characters.cbegin(), GetIteratorFindIfVector(characters, IsEndRule(terminal) ? NONTERMINAL_END_SEQUENCE : terminal));

			if ((row < transitions.size()) && (column < characters.size()) && (terminal != parentNonterminal))
			{
				transitions[row].second[column].second = true;
			}

			if ((nonterminal == parentNonterminal) && (outputData.nonterminal != nonterminal))
			{
				for (auto& data : outputDatas)
				{
					if (auto it2 = std::find_if(data.terminals.begin(), data.terminals.end(), [&](const std::string& str) {
							return str == outputData.nonterminal;
						}); it2 != data.terminals.end())
					{
						SearchStartingTerminalsEmptyRules(outputDatas, parentNonterminal, *it2, transitions, characters);
					}
				}
			}
		}
	}
}

void BuildingFirstRelationship(const std::vector<OutputDataGuideSets>& outputDatas, std::vector<PairStringVectorPair>& transitions, const std::vector<PairStringBool>& characters)
{
	for (auto& outputData : outputDatas)
	{
		size_t row = std::distance(transitions.cbegin(), GetIteratorFindIfVector(transitions, outputData.nonterminal));
		size_t column = std::distance(characters.cbegin(), GetIteratorFindIfVector(characters, outputData.terminals.front()));

		if ((row < transitions.size()) && (column < characters.size()))
		{
			if (IsEmptyRule(outputData.terminals.front()))
			{
				SearchStartingTerminalsEmptyRules(outputDatas, outputData.nonterminal, outputData.nonterminal, transitions, characters);
			}
			else
			{
				transitions[row].second[column].second = true;
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
				size_t row = std::distance(transitions.cbegin(), GetIteratorFindIfVector(transitions, (*it).second[j].first));

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

void AddingGuideCharacters(std::vector<OutputDataGuideSets>& outputDatas, const std::vector<std::string>& nonterminals, const std::vector<std::string>& terminals, bool bla = true)
{
	std::vector<PairStringVectorPair> transitions;
	std::vector<PairStringBool> characters;

	std::for_each(nonterminals.begin(), nonterminals.end(), [&](std::string str) { characters.emplace_back(str, false); });
	std::for_each(terminals.begin(), terminals.end(), [&](std::string str) { characters.emplace_back(str, false); });
	std::for_each(nonterminals.begin(), nonterminals.end(), [&](std::string str) { transitions.emplace_back(str, characters); });

	BuildingFirstRelationship(outputDatas, transitions, characters);
	BuildingFirstPlusRelationship(transitions);

	for (auto& outputData : outputDatas)
	{
		const auto [left, right, guide] = outputData;

		size_t row = std::distance(transitions.cbegin(), GetIteratorFindIfVector(transitions, outputData.nonterminal));

		const auto bla = std::find_if(outputDatas.cbegin(), outputDatas.cend(), [&](const OutputDataGuideSets& data) {
			return (data.nonterminal == outputData.nonterminal) && IsEmptyRule(data.terminals.front());
		}) != outputDatas.cend();

		if (row < transitions.size())
		{
			std::vector<std::string> guideCharacters;
			std::string firstTerminal = outputData.terminals.front();

			for (size_t i = nonterminals.size(); i < transitions[row].second.size(); ++i)
			{
				if (transitions[row].second[i].second)
				{
					std::string str = transitions[row].second[i].first;
					bool isEmptyRuleFirstTerminal = IsEmptyRule(firstTerminal);
					bool isEmptyRuleStr = IsEmptyRule(str);

					if (!str.empty() && (IsNonterminal(firstTerminal) || (firstTerminal == str) || isEmptyRuleFirstTerminal))
					{
						if (bla && isEmptyRuleStr && !isEmptyRuleFirstTerminal)
						{
							continue;
						}

						guideCharacters.push_back(isEmptyRuleStr ? TERMINAL_END_SEQUENCE : str);
						transitions[row].second[i].first = "";
					}
				}
			}
			outputData.guideCharacters = guideCharacters;
		}
	}
}

std::vector<OutputDataGuideSets> GetFormingGuideSets(std::istream& fileInput)
{
	std::vector<std::string> nonterminals;
	std::vector<std::string> terminals;

	std::vector<InputData> inputDatas;
	std::vector<OutputDataGuideSets> outputDatas;

	FillingData(fileInput, inputDatas, nonterminals, terminals);
	Forming(inputDatas, outputDatas, nonterminals, terminals);
	AddingGuideCharacters(outputDatas, nonterminals, terminals);

	return outputDatas;
}

void PrintResultGuideSets(std::ostream& output, const std::vector<OutputDataGuideSets>& outputDatas)
{
	for (const auto& outputData : outputDatas)
	{
		output << outputData.nonterminal << SPACE << DELIMITER << SPACE;
		PrintInfoVector(output, outputData.terminals, SPACE);
		output << SPACE << "/" << SPACE;
		PrintInfoVector(output, outputData.guideCharacters, SPACE);
		output << std::endl;
	}
}
