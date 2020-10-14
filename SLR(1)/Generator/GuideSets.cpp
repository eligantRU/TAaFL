#include "GuideSets.h"

#include "../../Lexer/Lexer.hpp"

namespace fromLL
{

void BuildingFirstRelationship(const std::vector<OutputDataGuideSets>& outputDatas, std::vector<PairStringVectorPair>& transitions, const std::vector<PairStringBool>& characters)
{
	for (auto& outputData : outputDatas)
	{
		size_t row = std::distance(transitions.cbegin(), GetIteratorFindIfVector(transitions, outputData.nonterminal));
		size_t column = std::distance(characters.cbegin(), GetIteratorFindIfVector(characters, outputData.terminals.front()));

		if ((row < transitions.size()) && (column < characters.size()))
		{
			transitions[row].second[column].second = true;
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

}

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

bool RemoveEmptyRulesImpl(std::vector<InputData>& inputDatas)
{
	std::set<std::string> epsables;
	std::vector<InputData> tmpInputDatas;
	tmpInputDatas.reserve(inputDatas.size());

	for (const auto & [left, right] : inputDatas)
	{
		if (const auto firstRight = right.front(); IsEmptyRule(firstRight) && (left != inputDatas.front().nonterminal))
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
			right.push_back(NONTERMINAL_END_SEQUENCE);
		}
	}

	inputDatas = std::move(tmpInputDatas);
	return epsables.empty();
}

void RemoveEmptyRules(std::vector<InputData>& inputDatas, std::vector<std::string>& terminals)
{
	while (!RemoveEmptyRulesImpl(inputDatas));

	terminals.erase(std::remove(terminals.begin(), terminals.end(), NONTERMINAL_END_SEQUENCE), terminals.end());
	terminals.push_back(TERMINAL_END_SEQUENCE);
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

void Bla(std::vector<OutputDataGuideSets>& outputDatas, const std::vector<std::string>& nonterminals, const std::vector<std::string>& terminals)
{
	std::vector<PairStringVectorPair> transitions;
	std::vector<PairStringBool> characters;

	std::for_each(nonterminals.begin(), nonterminals.end(), [&](std::string str) { characters.emplace_back(str, false); });
	std::for_each(terminals.begin(), terminals.end(), [&](std::string str) { characters.emplace_back(str, false); });
	std::for_each(nonterminals.begin(), nonterminals.end(), [&](std::string str) { transitions.emplace_back(str, characters); });

	fromLL::BuildingFirstRelationship(outputDatas, transitions, characters);
	fromLL::BuildingFirstPlusRelationship(transitions);

	std::cout << ">> Rules:" << std::endl;
	for (const auto& outputData : outputDatas)
	{
		std::cout << TAB << outputData.nonterminal << SPACE << DELIMITER << SPACE;
		PrintInfoVector(std::cout, outputData.terminals, SPACE);
		std::cout << std::endl;
	}
	std::cout << std::endl;
	/*std::cout << ">> First*:" << std::endl;
	for (const auto& nonTerminal : nonterminals)
	{
		const auto it = std::find_if(transitions.cbegin(), transitions.cend(), [&nonTerminal](const auto& row) {
			return row.first == nonTerminal;
		});

		std::vector<std::string> bla;
		for (const auto& [to, has] : (*it).second)
		{
			if (has)
			{
				bla.push_back(to);
			}
		}

		std::cout << TAB << nonTerminal << SPACE << DELIMITER << SPACE;
		PrintInfoVector(std::cout, bla, SPACE);
		std::cout << std::endl;
	}*/
}

bool IsReplenishedGrammar(const std::vector<OutputDataGuideSets>& outputDatas)
{
	for (const auto& outputData : outputDatas)
	{
		if (auto it = std::find(outputData.terminals.cbegin(), outputData.terminals.cend(), outputDatas.front().nonterminal);
			it != outputData.terminals.cend())
		{
			return false;
		}
	}
	return true;
}

void Forming(const std::vector<InputData>& inputDatas, std::vector<OutputDataGuideSets>& outputDatas, std::vector<std::string>& nonterminals)
{
	for (const auto& nonterminal : nonterminals)
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

		std::sort(temporaryVector.begin(), temporaryVector.end(), [](const InputData& a, const InputData& b) { return (a.terminals.front() != b.terminals.front()) && (a.terminals.front() == a.nonterminal); });
		std::for_each(temporaryVector.begin(), temporaryVector.end(), [&](const InputData& data) { outputDatas.push_back({ data.nonterminal, data.terminals }); });
	}

	if (!IsReplenishedGrammar(outputDatas))
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

	for (auto& [left, right, guide] : outputDatas)
	{
		if (const auto axiom = outputDatas.front().nonterminal; left == axiom)
		{
			if (IsEmptyRule(right.front()))
			{
				right.clear();
			}
			right.push_back(TERMINAL_END_SEQUENCE);
		}
	}
}

std::vector<OutputDataGuideSets> GetFormingGuideSets(std::istream& fileInput)
{
	std::vector<InputData> inputDatas;
	std::vector<OutputDataGuideSets> outputDatas;

	std::vector<std::string> nonterminals;
	std::vector<std::string> terminals;
	
	FillingData(fileInput, inputDatas, nonterminals, terminals);
	RemoveEmptyRules(inputDatas, terminals);
	Forming(inputDatas, outputDatas, nonterminals);

	Bla(outputDatas, nonterminals, terminals);

	return outputDatas;
}
