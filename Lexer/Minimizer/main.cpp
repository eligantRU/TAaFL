#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <memory>

#include "../Common/MachineData.hpp"
#include "../Common/IOUtils.hpp"

using namespace std;

bool TransitionExist(MachineData const& machine, MachineState src, MachineState dest)
{
	for (InputSignal in = 0; in < machine.m_inputAmount; ++in)
	{
		switch (machine.m_typedData.GetType())
		{
		case MachineType::MEALY:
		{
			const MealyData& mealyData = machine.m_typedData.GetMealyData();
			if (mealyData(in, src).state == dest)
			{
				return true;
			}
			break;
		}

		default:
			assert(false);
			return false;
		}
	}
	return false;
}

vector<bool> GetReachableStates(MachineData const& machine)
{
	vector<bool> reachableStates(machine.m_stateAmount, false);
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

size_t CountTrueValues(vector<bool> const& data)
{
	return std::count(data.cbegin(), data.cend(), true);
}

unique_ptr<MachineData> SaveSpecifiedStates(MachineData const& machine, vector<bool> const& states)
{
	const MachineState stateCount = CountTrueValues(states);

	vector<MachineState> newStateFunction(machine.m_stateAmount, machine.m_stateAmount);
	MachineState newState = 0;
	for (MachineState oldState = 0; oldState < machine.m_stateAmount; ++oldState)
	{
		if (states[oldState])
		{
			newStateFunction[oldState] = newState;
			++newState;
		}
	}

	unique_ptr<MachineData> result(new MachineData(machine.m_typedData.GetType(), machine.m_inputAmount, machine.m_outputAmount, stateCount));

	switch (machine.m_typedData.GetType())
	{
	case MachineType::MEALY:
	{
		MealyData const& oldMealyData = machine.m_typedData.GetMealyData();
		MealyData& newMealyData = result->m_typedData.GetMealyData();

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
		assert(false);
	}
	return result;
}

unique_ptr<MachineData> RemoveUnreachableStates(MachineData const& machine)
{
	return SaveSpecifiedStates(machine, GetReachableStates(machine));
}

bool StatesAreEquivalent(MachineData const& machine, MachineState a, MachineState b)
{
	switch (machine.m_typedData.GetType())
	{
	case MachineType::MEALY:
	{
		MealyData const& mealyData = machine.m_typedData.GetMealyData();

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
		assert(false);
		return false;
	}
}

void ReplaceStates(MachineData& machine, MachineState find, MachineState replace)
{
	for (MachineState st = 0; st < machine.m_stateAmount; ++st)
	{
		for (InputSignal in = 0; in < machine.m_inputAmount; ++in)
		{
			switch (machine.m_typedData.GetType())
			{
			case MachineType::MEALY:
			{
				MealyData& mealyData = machine.m_typedData.GetMealyData();

				if (mealyData(in, st).state == find)
				{
					mealyData(in, st).state = replace;
				}

				break;
			}

			default:
				assert(false);
			}
		}
	}
}

unique_ptr<MachineData> MergeEquivalentStates(MachineData machine)
{
	vector<bool> activeStates(machine.m_stateAmount, true);

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

unique_ptr<MachineData> MinimizeMachine(MachineData const& machine)
{
	return MergeEquivalentStates(*RemoveUnreachableStates(machine));
}

int main(int, char* [])
{
	try
	{
		GetMachineData(cout, *MinimizeMachine(*GetMachineData(cin)));
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
