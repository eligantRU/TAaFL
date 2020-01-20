#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <set>

#include "../Common/GraphizUtils.hpp"

using namespace::std;

template <class T>
void UnionSet(set<T> & targ, const set<T> & add)
{
	targ.insert(add.cbegin(), add.cend());
}

struct NFA
{
	vector<set<string>> states;
	vector<string> inputs;
	string startState;
	set<string> acceptStates;
	vector<vector<set<string>>> table;

	static constexpr size_t BAD_INDEX = SIZE_MAX;

	size_t GetStateIndex(const set<string> & state) const
	{
		for (size_t i = 0; i < states.size(); ++i)
		{
			if (state == states[i])
			{
				return i;
			}
		}
		return BAD_INDEX;
	}

	size_t GetInputIndex(const string & str) const
	{
		for (size_t i = 0; i < inputs.size(); ++i)
		{
			if (str == inputs[i])
			{
				return i;
			}
		}
		return BAD_INDEX;
	}

	void InsertState(const set<string> & srcState, const string & input, const set<string> & targState)
	{
		const size_t srcStateIndex = GetStateIndex(srcState);
		const size_t inputIndex = GetInputIndex(input);
		assert(srcStateIndex != BAD_INDEX && inputIndex != BAD_INDEX);
		UnionSet(table[srcStateIndex][inputIndex], targState);
	}

	void AddPowersetState(const set<string> & powerState)
	{
		states.push_back(powerState);

		const vector<set<string>> emptyNode(inputs.size());
		table.push_back(emptyNode);

		for (const auto & state : powerState)
		{
			const size_t stateInd = GetStateIndex({ state });
			assert(stateInd != BAD_INDEX);

			for (size_t in = 0; in < inputs.size(); ++in)
			{
				UnionSet(table.back()[in], table[stateInd][in]);
			}
		}
	}
};

vector<string> GetInBraceParts(const string & str, char lbrace, char rbrace)
{
	vector<string> result;
	size_t startFrom = 0;
	while (startFrom < str.length())
	{
		const size_t begin = str.find(lbrace, startFrom);
		const size_t end = str.find(rbrace, startFrom);
		const bool dataFound = (begin != string::npos && end != string::npos && begin < end);

		if (dataFound)
		{
			const size_t dataLength = end - begin - 1;
			result.push_back(str.substr(begin + 1, dataLength));
			startFrom = end + 1;
		}
		else
		{
			startFrom = string::npos;
		}
	}
	return result;
}

vector<string> SplitString(const string & str, const string & delimiter)
{
	vector<string> result;
	size_t startIndex = 0;
	while (startIndex < str.length())
	{
		const size_t delimiterIndex = str.find(delimiter, startIndex);
		const bool delimiterFound = (delimiterIndex != string::npos);
		const size_t partLength = delimiterFound ? delimiterIndex - startIndex : string::npos;

		result.push_back(str.substr(startIndex, partLength));
		startIndex = delimiterFound ? delimiterIndex + delimiter.length() : string::npos;
	}
	return result;
}

vector<set<string>> StrVectorToStrSetVector(const vector<string> & strVector)
{
	vector<set<string>> result;
	for (auto str : strVector)
	{
		result.push_back({ str });
	}
	return result;
}

set<string> ToSet(const vector<string> & strVector)
{
	return { strVector.cbegin(), strVector.cend() };
}

string GetInBracePart(const string & str, char lbrace, char rbrace)
{
	const vector<string> parts = GetInBraceParts(str, lbrace, rbrace);
	assert(parts.size() == 1);
	return parts[0];
}

void ParseTransition(const string & str, string & srcState, string & input, string & targState)
{
	const vector<string> parts = SplitString(str, " = ");
	assert(parts.size() == 2);

	const vector<string> leftPartData = SplitString(GetInBracePart(str, '(', ')'), ", ");
	assert(leftPartData.size() == 2);

	srcState = leftPartData[0];
	input = leftPartData[1];
	targState = parts[1];
}

NFA ReadFromFile(istream & input)
{
	NFA result;
	{
		string definition;
		getline(input, definition);

		const vector<string> defParts = GetInBraceParts(definition, '{', '}');
		assert(defParts.size() == 4);

		result.states = StrVectorToStrSetVector(SplitString(defParts[0], ", "));
		result.inputs = SplitString(defParts[1], ", ");
		result.startState = defParts[2];
		result.acceptStates = ToSet(SplitString(defParts[3], ", "));
	}

	{
		const vector<set<string>> emptyNode(result.inputs.size());
		result.table.assign(result.states.size(), emptyNode);
	}

	string curLine;
	while (getline(input, curLine))
	{
		string srcState, input, targState;
		ParseTransition(curLine, srcState, input, targState);
		result.InsertState({ srcState }, input, { targState });
	}
	return result;
}

void DefineAllPowersetStates(NFA & nfa)
{
	for (size_t st = 0; st < nfa.table.size(); ++st)
	{
		assert(nfa.states.size() == nfa.table.size());
		for (size_t in = 0; in < nfa.inputs.size(); ++in)
		{
			if (!nfa.table[st][in].empty() && nfa.GetStateIndex(nfa.table[st][in]) == NFA::BAD_INDEX)
			{
				nfa.AddPowersetState(nfa.table[st][in]);
			}
		}
	}
}

bool IsStateAccepting(const set<string> & state, const set<string> & acceptingStates)
{
	for (const auto & acceptingState : acceptingStates)
	{
		if (state.find(acceptingState) != state.cend())
		{
			return true;
		}
	}
	return false;
}

vector<bool> GetAcceptingStates(const NFA & nfa)
{
	vector<bool> result(nfa.states.size(), false);
	for (size_t st = 0; st < nfa.states.size(); ++st)
	{
		if (IsStateAccepting(nfa.states[st], nfa.acceptStates))
		{
			result[st] = true;
		}
	}
	return result;
}

string IndexToNewState(size_t index)
{
	string result;
	if (index < 26)
	{
		result = char('A' + index);
	}
	else
	{
		result = "S" + to_string(index - 26);
	}
	return result;
}

void PrintAsDFA(ostream & output, const NFA & nfa)
{
	output << "M = ({";
	for (size_t st = 0; st < nfa.states.size(); ++st)
	{
		if (st)
		{
			output << ", ";
		}
		output << IndexToNewState(st);
	}
	output << "}, {";
	for (size_t in = 0; in < nfa.inputs.size(); ++in)
	{
		if (in)
		{
			output << ", ";
		}
		output << nfa.inputs[in];
	}
	output << "}, F, {" << IndexToNewState(nfa.GetStateIndex({ nfa.startState })) << "}, {";
	const vector<bool> acceptingStates = GetAcceptingStates(nfa);
	bool alreadyPrinted = false;
	for (size_t st = 0; st < nfa.states.size(); ++st)
	{
		if (acceptingStates[st])
		{
			if (alreadyPrinted)
			{
				output << ", ";
			}
			output << IndexToNewState(st);
			alreadyPrinted = true;
		}
	}
	output << "})\n";

	std::vector<WeightedEdge> weightedEdges;
	for (size_t st = 0; st < nfa.states.size(); ++st)
	{
		for (size_t in = 0; in < nfa.inputs.size(); ++in)
		{
			if (!nfa.table[st][in].empty())
			{
				weightedEdges.emplace_back(Edge{ st, nfa.GetStateIndex(nfa.table[st][in]) }, nfa.inputs[in]);
				output << "F(" << IndexToNewState(st) << ", " << nfa.inputs[in] << ") = " << IndexToNewState(nfa.GetStateIndex(nfa.table[st][in])) << "\n";
			}
		}
	}
	ToGraphizFormat(weightedEdges);
}

int main()
{
	NFA nfa = ReadFromFile(cin);
	DefineAllPowersetStates(nfa);
	PrintAsDFA(cout, nfa);
}
