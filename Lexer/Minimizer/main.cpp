#include <iostream>
#include <fstream>
#include <istream>
#include <cassert>
#include <vector>
#include <string>

#include "../Common/GraphizConverter.hpp"
#include "../Common/MachineData.hpp"
#include "../Common/IOUtils.hpp"

bool TransitionExist(const MachineData & machine, MachineState src, MachineState dest)
{
	for (InputSignal in = 0; in < machine.m_inputAmount; ++in)
	{
		switch (machine.m_typedData.GetType())
		{
		case MachineType::MOORE:
		{
			const MooreData & mooreData = machine.m_typedData.GetMooreData();
			if (mooreData.m_table(in, src) == dest)
			{
				return true;
			}
			break;
		}

		case MachineType::MEALY:
		{
			const MealyData & mealyData = machine.m_typedData.GetMealyData();
			if (mealyData(in, src).state == dest)
			{
				return true;
			}
			break;
		}

		default:
			throw std::invalid_argument("Unexpected automata type");
		}
	}
	return false;
}

std::vector<bool> GetReachableStates(const MachineData & machine)
{
	std::vector<bool> reachableStates(machine.m_stateAmount, false);
	reachableStates[0] = true;

	bool foundNewStates;
	do
	{
		foundNewStates = false;

		for (MachineState src = 0; src < machine.m_stateAmount && !foundNewStates; ++src)
		{
			if (reachableStates[src])
			{
				for (MachineState dest = 0; dest < machine.m_stateAmount && !foundNewStates; ++dest)
				{
					if (!reachableStates[dest] && TransitionExist(machine, src, dest))
					{
						foundNewStates = true;
						reachableStates[dest] = true;
					}
				}
			}
		}
	} while (foundNewStates);

	return reachableStates;
}

std::unique_ptr<MachineData> SaveSpecifiedStates(const MachineData & machine, const std::vector<bool> & states)
{
	const MachineState stateCount = std::count(states.cbegin(), states.end(), true);

	std::vector<MachineState> newStateFunction(machine.m_stateAmount, machine.m_stateAmount);
	MachineState newState = 0;
	for (MachineState oldState = 0; oldState < machine.m_stateAmount; ++oldState)
	{
		if (states[oldState])
		{
			newStateFunction[oldState] = newState;
			++newState;
		}
	}

	std::unique_ptr<MachineData> result(new MachineData(machine.m_typedData.GetType(), machine.m_inputAmount, machine.m_outputAmount, stateCount));

	switch (machine.m_typedData.GetType())
	{
	case MachineType::MOORE:
	{
		const MooreData & oldMooreData = machine.m_typedData.GetMooreData();
		MooreData & newMooreData = result->m_typedData.GetMooreData();

		MachineState newState = 0;
		for (MachineState oldState = 0; oldState < machine.m_stateAmount; ++oldState)
		{
			if (states[oldState])
			{
				newMooreData.m_output[newState] = oldMooreData.m_output[oldState];
				for (InputSignal in = 0; in < machine.m_inputAmount; ++in)
				{
					newMooreData.m_table(in, newState) = newStateFunction[oldMooreData.m_table(in, oldState)];
				}
				++newState;
			}
		}
		break;
	}

	case MachineType::MEALY:
	{
		const MealyData & oldMealyData = machine.m_typedData.GetMealyData();
		MealyData & newMealyData = result->m_typedData.GetMealyData();

		MachineState newState = 0;
		for (MachineState oldState = 0; oldState < machine.m_stateAmount; ++oldState)
		{
			if (states[oldState])
			{
				for (InputSignal in = 0; in < machine.m_inputAmount; ++in)
				{
					newMealyData(in, newState).output = oldMealyData(in, oldState).output;
					newMealyData(in, newState).state = newStateFunction[oldMealyData(in, oldState).state];
				}
				++newState;
			}
		}
		break;
	}

	default:
		throw std::invalid_argument("Unexpected automata type");
	}

	return result;
}

std::unique_ptr<MachineData> RemoveUnreachableStates(const MachineData & machine)
{
	return SaveSpecifiedStates(machine, GetReachableStates(machine));
}

bool StatesAreEquivalent(const MachineData & machine, MachineState a, MachineState b)
{
	switch (machine.m_typedData.GetType())
	{
	case MachineType::MOORE:
	{
		const MooreData & mooreData = machine.m_typedData.GetMooreData();
		if (mooreData.m_output[a] != mooreData.m_output[b])
		{
			return false;
		}

		for (InputSignal in = 0; in < machine.m_inputAmount; ++in)
		{
			if (mooreData.m_table(in, a) != mooreData.m_table(in, b))
			{
				return false;
			}
		}
		return true;
	}

	case MachineType::MEALY:
	{
		const MealyData & mealyData = machine.m_typedData.GetMealyData();
		for (InputSignal in = 0; in < machine.m_inputAmount; ++in)
		{
			if (mealyData(in, a) != mealyData(in, b))
			{
				return false;
			}
		}
		return true;
	}

	default:
		throw std::invalid_argument("Unexpected automata type");
	}
}

void ReplaceStates(MachineData & machine, MachineState find, MachineState replace)
{
	for (MachineState st = 0; st < machine.m_stateAmount; ++st)
	{
		for (InputSignal in = 0; in < machine.m_inputAmount; ++in)
		{
			switch (machine.m_typedData.GetType())
			{
			case MachineType::MOORE:
			{
				MooreData & mooreData = machine.m_typedData.GetMooreData();
				if (mooreData.m_table(in, st) == find)
				{
					mooreData.m_table(in, st) = replace;
				}
				break;
			}

			case MachineType::MEALY:
			{
				MealyData & mealyData = machine.m_typedData.GetMealyData();
				if (mealyData(in, st).state == find)
				{
					mealyData(in, st).state = replace;
				}
				break;
			}

			default:
				throw std::invalid_argument("Unexpected automata type");
			}
		}
	}
}

std::unique_ptr<MachineData> MergeEquivalentStates(MachineData machine)
{
	std::vector<bool> activeStates(machine.m_stateAmount, true);

	bool changesMade;
	do
	{
		changesMade = false;

		for (MachineState a = 0; a < machine.m_stateAmount && !changesMade; ++a)
		{
			if (activeStates[a])
			{
				for (MachineState b = a + 1; b < machine.m_stateAmount && !changesMade; ++b)
				{
					if (activeStates[b] && StatesAreEquivalent(machine, a, b))
					{
						changesMade = true;
						activeStates[b] = false;
						ReplaceStates(machine, b, a);
					}
				}
			}
		}
	} while (changesMade);

	return SaveSpecifiedStates(machine, activeStates);
}

std::unique_ptr<MachineData> MinimizeMachine(const MachineData & machine)
{
	auto minimizedMachine = MergeEquivalentStates(*RemoveUnreachableStates(machine));
	if (minimizedMachine->m_typedData.GetType() == MachineType::MEALY)
	{
		RenderMealy(*minimizedMachine);
	}
	return minimizedMachine;
}

int main()
{
	try
	{
		GetMachineData(std::cout, *MinimizeMachine(*GetMachineData(std::cin)));
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
