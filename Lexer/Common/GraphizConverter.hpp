#include <vector>

#include "GraphizUtils.hpp"
#include "MachineData.hpp"

void RenderMealy(const MachineData & mealy)
{
	const auto mealyData = mealy.m_typedData.GetMealyData();

	std::vector<Edge> edges;
	for (size_t i = 0; i < mealyData.GetRowCount(); ++i)
	{
		for (size_t j = 0; j < mealy.m_stateAmount; ++j)
		{
			edges.emplace_back(mealyData(i, j).state, mealyData(i, j).output);
		}
	}

	std::vector<WeightedEdge> weightedEdges;
	for (size_t i = 0, x = 0, index = 0; i < edges.size(); ++i, ++index)
	{
		if ((i % mealy.m_stateAmount == 0) && (i != 0))
		{
			++x;
			index = 0;
		}
		weightedEdges.emplace_back(Edge{ index, edges[i].first }, "x" + std::to_string(x) + "y" + std::to_string(edges[i].second));
	}
	ToGraphizFormat(weightedEdges);
}

void RenderMoore(const MachineData & mealy, const MealyData & mealyData)
{
	std::vector<size_t> a;
	for (size_t i = 0; i < mealyData.GetRowCount(); ++i)
	{
		for (size_t j = 0; j < mealyData.GetColumnCount(); ++j)
		{
			a.push_back(mealyData(i, j).state);
			a.push_back(mealyData(i, j).output);
		}
	}

	std::vector<Edge> inputEdges;
	inputEdges.reserve(a.size() / 2);
	for (size_t i = 1; i < a.size(); i += 2)
	{
		inputEdges.emplace_back(a[i - 1], a[i]);
	}

	std::vector<Edge> uniqueEdges(inputEdges.begin(), inputEdges.end());
	std::sort(uniqueEdges.begin(), uniqueEdges.end());

	uniqueEdges.erase(std::unique(uniqueEdges.begin(), uniqueEdges.end()), uniqueEdges.end());

	std::vector<size_t> outputState(mealy.m_inputAmount * uniqueEdges.size(), 0);

	for (size_t i = 0; i < uniqueEdges.size(); ++i)
	{
		size_t indexEdge = i;
		int index = uniqueEdges[i].first;

		for (size_t j = 0; j < mealy.m_inputAmount; ++j)
		{
			const auto it = std::find(uniqueEdges.begin(), uniqueEdges.end(), inputEdges[index]);
			outputState[indexEdge] = int(std::distance(uniqueEdges.begin(), it));

			if (j < mealy.m_inputAmount - 1)
			{
				indexEdge += uniqueEdges.size();
				index += mealy.m_stateAmount;
			}
		}
	}

	std::vector<WeightedEdge> weightedEdges;
	weightedEdges.reserve(outputState.size());
	for (size_t i = 0, x = 0, index = 0; i < outputState.size(); ++i, ++index)
	{
		if ((i % uniqueEdges.size() == 0) && (i != 0))
		{
			++x;
			index = 0;
		}

		weightedEdges.emplace_back(Edge{ index, outputState[i] }, "x" + std::to_string(x));
	}

	ToGraphizFormat(weightedEdges);
}
