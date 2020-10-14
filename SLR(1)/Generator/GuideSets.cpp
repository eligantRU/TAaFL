#include "GuideSets.h"

#include "../../Lexer/Lexer.hpp"

namespace
{

namespace fromLL
{

template <class T>
T Uniqify(const T & c)
{
	std::set bla(c.cbegin(), c.cend());
	return { bla.cbegin(), bla.cend() };
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

std::vector<std::string> GetFollow(const std::vector<OutputDataGuideSets>& rules, std::string nonTerminal, std::set<std::string> way = {})
{
	std::vector<std::string> result;
	for (const auto& subRule : rules)
	{
		if (auto it = std::find_if(subRule.terminals.cbegin(), subRule.terminals.cend(), [&](std::string_view sv) {
			return sv == nonTerminal;
			}); it != subRule.terminals.cend())
		{
			size_t distance = std::distance(subRule.terminals.cbegin(), it);
			size_t size = subRule.terminals.size() - 1;

			if (const auto bla = (distance <= size)
				? ((distance < size) ? subRule.terminals[distance + 1] : subRule.terminals.back())
				: NONTERMINAL_END_SEQUENCE; bla != nonTerminal)
			{
				result.push_back(bla);

				if ((bla == TERMINAL_END_SEQUENCE) && !way.count(subRule.nonterminal)) // TODO: copy-paste
				{
					auto tmpWay(way);
					tmpWay.insert(subRule.nonterminal);
					const auto tmp = GetFollow(rules, subRule.nonterminal, tmpWay);
					std::copy(tmp.cbegin(), tmp.cend(), std::back_inserter(result));
				}
			}
			else if (!way.count(subRule.nonterminal))
			{
				auto tmpWay(way);
				tmpWay.insert(subRule.nonterminal);
				const auto tmp = GetFollow(rules, subRule.nonterminal, tmpWay);
				std::copy(tmp.cbegin(), tmp.cend(), std::back_inserter(result));
			}
		}
	}
	return Uniqify(result);
}

std::vector<std::string> GetFirst(const std::vector<OutputDataGuideSets> & rules, OutputDataGuideSets processingRule)
{
	std::vector<std::string> result;

	const auto processingLeft = processingRule.nonterminal;
	const auto firstProcessingRight = processingRule.terminals.front();
	
	if (IsEmptyRule(firstProcessingRight))
	{
		const auto bla = GetFollow(rules, processingLeft);
		std::copy(bla.cbegin(), bla.cend(), std::back_inserter(result));
	}
	else if (IsNonterminal(firstProcessingRight))
	{
		for (const auto& rule : rules)
		{
			if (IsNonterminal(firstProcessingRight))
			{
				if (rule.nonterminal == firstProcessingRight)
				{
					const auto bla = GetFirst(rules, rule);
					std::copy(bla.cbegin(), bla.cend(), std::back_inserter(result));
				}
			}
		}
	}
	else
	{
		result = { firstProcessingRight };
	}
	return Uniqify(result);
}

std::vector<std::string> GetGuideCharsByRule(const std::vector<OutputDataGuideSets> & rules, OutputDataGuideSets processingRule)
{
	auto result = GetFirst(rules, processingRule);

	decltype(result) terminals;
	decltype(result) nonTerminals;

	auto it = std::partition(result.begin(), result.end(), [](const std::string& sv) {
		return IsNonterminal(sv);
	});
	std::copy(result.begin(), it, std::back_inserter(nonTerminals));
	std::copy(it, result.end(), std::back_inserter(terminals));
	result = terminals;

	for (const auto & nonTerminal : nonTerminals)
	{
		for (const auto & rule : rules)
		{
			if (rule.nonterminal == nonTerminal)
			{
				const auto bla = GetGuideCharsByRule(rules, rule);
				std::copy(bla.cbegin(), bla.cend(), std::back_inserter(result));
			}
		}
	}
	return Uniqify(result);
};

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

void RemoveEmptyRules(std::vector<InputData>& inputDatas)
{
	while (!RemoveEmptyRulesImpl(inputDatas));
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

void AddingGuideCharacters(std::vector<OutputDataGuideSets>& outputDatas)
{
	for (auto & outputData : outputDatas)
	{
		outputData.guideCharacters = fromLL::GetGuideCharsByRule(outputDatas, outputData);
	}
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
	RemoveEmptyRules(inputDatas);
	Forming(inputDatas, outputDatas, nonterminals);

	AddingGuideCharacters(outputDatas);

	return outputDatas;
}

void PrintResultGuideSets(std::ostream& fileOutput, const std::vector<OutputDataGuideSets>& outputDatas)
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
