#include <iostream>
#include <cassert>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <map>

#include "../Common/GraphizConverter.hpp"
#include "../Common/MachineData.hpp"
#include "../Common/IOUtils.hpp"

std::unique_ptr<MachineData> MealyToMoore(const MachineData & mealy)
{
	const MealyData & mealyData = mealy.m_typedData.GetMealyData();

	using NewStateFunction = std::map<OutputState, MachineState>;
	NewStateFunction newStateFunction;

	for (InputSignal in = 0; in < mealy.m_inputAmount; ++in)
	{
		for (MachineState st = 0; st < mealy.m_stateAmount; ++st)
		{
			newStateFunction.insert(NewStateFunction::value_type(mealyData(in, st), 0));
		}
	}

	MachineState curState = 0;
	for (auto & [_, machineState] : newStateFunction)
	{
		machineState = curState++;
	}

	auto moore = std::make_unique<MachineData>(MachineType::MOORE, mealy.m_inputAmount, mealy.m_outputAmount, newStateFunction.size());
	MooreData & mooreData = moore->m_typedData.GetMooreData();

	MachineState targState = 0;
	for (auto & [outputState, _] : newStateFunction)
	{
		const MachineState srcState = outputState.state;

		for (InputSignal curInput = 0; curInput < moore->m_inputAmount; ++curInput)
		{
			mooreData.m_table(curInput, targState) = newStateFunction[mealyData(curInput, srcState)];
		}

		mooreData.m_output[targState++] = outputState.output;
	}

	RenderMoore(mealy, mealyData);

	return moore;
}

std::unique_ptr<MachineData> MooreToMealy(const MachineData & moore)
{
	const MooreData & mooreData = moore.m_typedData.GetMooreData();

	auto mealy = std::make_unique<MachineData>(MachineType::MEALY, moore.m_inputAmount, moore.m_outputAmount, moore.m_stateAmount);
	MealyData & mealyData = mealy->m_typedData.GetMealyData();

	for (InputSignal in = 0; in < moore.m_inputAmount; ++in)
	{
		for (MachineState st = 0; st < moore.m_stateAmount; ++st)
		{
			mealyData(in, st).state = mooreData.m_table(in, st);
			mealyData(in, st).output = mooreData.m_output[mealyData(in, st).state];
		}
	}

	RenderMealy(*mealy);

	return mealy;
}

void Convert(std::istream & input, std::ostream & output)
{
	const std::unique_ptr<MachineData> inputMachine = GetMachineData(input);

	std::unique_ptr<MachineData> outputMachine;
	switch (inputMachine->m_typedData.GetType())
	{
	case MachineType::MOORE:
		outputMachine = MooreToMealy(*inputMachine);
		break;
	case MachineType::MEALY:
		outputMachine = MealyToMoore(*inputMachine);
		break;
	default:
		throw std::invalid_argument("Unexpected automata type");
	}

	GetMachineData(output, *outputMachine);
}

int main()
{
	try
	{
		Convert(std::cin, std::cout);
	}
	catch (const std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown error" << std::endl;
	}
}
