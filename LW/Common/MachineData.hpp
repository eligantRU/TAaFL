#pragma once

#include <variant>

#include "Matrix.hpp"

using InputSignal = size_t;
using OutputSignal = size_t;
using MachineState = size_t;

enum class MachineType
{
	MOORE,
	MEALY,
};

class MooreData
{
public:
	Matrix<MachineState> m_table;
	std::vector<OutputSignal> m_output;

	MooreData(InputSignal inputAmount, MachineState stateAmount)
		:m_table(inputAmount, stateAmount, 0)
		,m_output(stateAmount, 0)
	{}
};

struct OutputState
{
	MachineState state;
	OutputSignal output;

	bool operator<(const OutputState & other) const noexcept
	{
		return (state == other.state)
			? (output < other.output)
			: (state < other.state);
	}

	bool operator!=(OutputState const& other) const noexcept
	{
		return (state != other.state || output != other.output);
	}
};
using MealyData = Matrix<OutputState>;

class MachineTypedData
{
public:
	MachineTypedData(MachineType type, InputSignal inputAmount, MachineState stateAmount)
		:m_type(type)
	{
		switch (type)
		{
		case MachineType::MOORE:
			m_data = MooreData(inputAmount, stateAmount);
			break;
		case MachineType::MEALY:
			m_data = MealyData(inputAmount, stateAmount, { 0, 0 });
			break;
		default:
			throw std::invalid_argument("Unexpected automata type");
		}
	}

	MachineType GetType() const noexcept
	{
		return m_type;
	}

	MooreData & GetMooreData()
	{
		return std::get<MooreData>(m_data);
	}

	const MooreData & GetMooreData() const
	{
		return std::get<MooreData>(m_data);
	}

	MealyData & GetMealyData()
	{
		return std::get<MealyData>(m_data);
	}

	const MealyData & GetMealyData() const
	{
		return std::get<MealyData>(m_data);
	}

private:
	MachineType m_type;
	std::variant<std::monostate, MealyData, MooreData> m_data;
};

class MachineData
{
public:
	InputSignal m_inputAmount;
	OutputSignal m_outputAmount;
	MachineState m_stateAmount;
	MachineTypedData m_typedData;

	MachineData(MachineType type, InputSignal inputAmount, OutputSignal outputAmount, MachineState stateAmount)
		:m_inputAmount(inputAmount)
		,m_outputAmount(outputAmount)
		,m_stateAmount(stateAmount)
		,m_typedData(type, inputAmount, stateAmount)
	{}
};
