#include <iostream>
#include <cassert>
#include <sstream>
#include <vector>
#include <string>

#include "../Common/MachineData.hpp"

MachineType ParseTypeStr(const std::string & str)
{
	if (str == "moore")
	{
		return MachineType::MOORE;
	}
	else if (str == "mealy")
	{
		return MachineType::MEALY;
	}
	else
	{
		throw std::invalid_argument("Unexpected automata type: " + str);
	}
}

std::vector<std::string> SplitString(const std::string & str, char delimiter)
{
	std::vector<std::string> result;

	size_t startFrom = 0;
	size_t findResult;
	while (startFrom < str.length() && ((findResult = str.find(delimiter, startFrom)) != std::string::npos))
	{
		assert(findResult >= startFrom);
		result.push_back(str.substr(startFrom, findResult - startFrom));
		startFrom = findResult + 1;
	}
	result.push_back((startFrom < str.length()) ? str.substr(startFrom) : "");
	return result;
}

std::vector<std::string> Split_witString(const std::string & str)
{
	std::istringstream strm(str);
	
	char _0, _2;
	size_t _1, _3;

	strm >> _0 >> _1 >> _2 >> _3;
	return {
		std::string(1, _0) + std::to_string(_1),
		std::string(1, _2) + std::to_string(_3),
	};
}

std::vector<std::string> GetRowElements(std::istream & stream)
{
	std::string rowStr;
	std::getline(stream, rowStr);
	return SplitString(rowStr, ' ');
}

size_t GetNodeNumber(const std::string & str)
{
	assert(str.length() > 1);
	return std::stoi(str.substr(1));
}

std::unique_ptr<MachineData> GetMachineData(std::istream & input)
{
	assert(!(input.rdstate() & std::istream::failbit));

	InputSignal inputAmount;
	input >> inputAmount;

	OutputSignal outputAmount;
	input >> outputAmount;

	MachineState stateAmount;
	input >> stateAmount;

	std::string typeStr;
	input >> typeStr;
	const MachineType type = ParseTypeStr(typeStr);
	std::getline(input, typeStr);

	auto result = std::make_unique<MachineData>(type, inputAmount, outputAmount, stateAmount);
	switch (type)
	{
	case MachineType::MOORE:
	{
		auto & moore = result->m_typedData.GetMooreData();

		const auto outputStrings = GetRowElements(input);
		for (MachineState st = 0; st < stateAmount; ++st)
		{
			moore.m_output[st] = GetNodeNumber(outputStrings[st]);
		}

		for (InputSignal curInput = 0; curInput < inputAmount; ++curInput)
		{
			const auto transitionStrings = GetRowElements(input);
			for (size_t curState = 0; curState < stateAmount; ++curState)
			{
				moore.m_table(curInput, curState) = GetNodeNumber(transitionStrings[curState]);
			}
		}
		break;
	}

	case MachineType::MEALY:
	{
		MealyData & mealy = result->m_typedData.GetMealyData();

		for (InputSignal curInput = 0; curInput < inputAmount; ++curInput)
		{
			const auto transitionStrings = GetRowElements(input);
			for (size_t curState = 0; curState < stateAmount; ++curState)
			{
				const auto components = Split_witString(transitionStrings[curState]);

				mealy(curInput, curState).state = GetNodeNumber(components[0]);
				mealy(curInput, curState).output = GetNodeNumber(components[1]);
			}
		}
		break;
	}

	default:
		throw std::invalid_argument("Unexpected automata type");
	}
	return result;
}

void GetMachineData(std::ostream & output, const MachineData & data)
{
#ifdef DEBUG
	output << data.m_inputAmount << " " << data.m_outputAmount << " " << data.m_stateAmount << std::endl;
#endif

	switch (data.m_typedData.GetType())
	{
	case MachineType::MOORE:
	{
		output << "moore" << std::endl;
		const MooreData & mooreData = data.m_typedData.GetMooreData();
		for (InputSignal in = 0; in < data.m_inputAmount; ++in)
		{
			for (MachineState st = 0; st < data.m_stateAmount; ++st)
			{
				output << "z" << mooreData.m_table(in, st);
				if (st + 1 < data.m_stateAmount)
				{
					output << " ";
				}
			}
			output << std::endl;
		}
		break;
	}

	case MachineType::MEALY:
	{
		output << "mealy" << std::endl;
		const MealyData & mealyData = data.m_typedData.GetMealyData();
		for (InputSignal in = 0; in < data.m_inputAmount; ++in)
		{
			for (MachineState st = 0; st < data.m_stateAmount; ++st)
			{
				output << "q" << mealyData(in, st).state << "y" << mealyData(in, st).output;
				if (st + 1 < data.m_stateAmount)
				{
					output << " ";
				}
			}
			output << std::endl;
		}
		break;
	}

	default:
		throw std::invalid_argument("Unexpected automata type");
	}
}
