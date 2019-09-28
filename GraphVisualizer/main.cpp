#include <exception>
#include <algorithm>
#include <fstream>
#include <string>

#pragma warning(push, 3)
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/format.hpp>
#pragma warning(pop)

using EdgeWeightType = std::string;

using Edge = std::pair<size_t, size_t>;
using WeightedEdge = std::pair<Edge, EdgeWeightType>;
using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::property<boost::vertex_color_t, boost::default_color_type>, boost::property<boost::edge_weight_t, EdgeWeightType>>;

namespace
{

size_t CountVertices(const std::vector<Edge>& edges)
{
	std::set<size_t> uniqueVerices;

	for (const auto & [firstVertex, secondVertex] : edges)
	{
		uniqueVerices.emplace(firstVertex);
		uniqueVerices.emplace(secondVertex);
	}
	return uniqueVerices.size();
}

}

void ToGraphizFormat(const std::vector<Edge>& edges, const std::vector<EdgeWeightType>& weights)
{
	Graph graph(edges.begin(), edges.end(), weights.begin(), CountVertices(edges));

	boost::dynamic_properties dp;
	dp.property("weight", boost::get(boost::edge_weight, graph));
	dp.property("label", boost::get(boost::edge_weight, graph));
	dp.property("node_id", boost::get(boost::vertex_index, graph));

	constexpr auto outputFileName = "graphizFormatedGraph.dot";
	std::ofstream ofs(outputFileName);
	if (!ofs.is_open())
	{
		throw std::runtime_error((boost::format("Cannot open file '%1%'") % outputFileName).str());
	}

	boost::write_graphviz_dp(ofs, graph, dp);
}

void ToGraphizFormat(const std::vector<WeightedEdge> & weightedEdges)
{
	std::vector<Edge> edges;
	edges.reserve(weightedEdges.size());

	std::vector<EdgeWeightType> weights;
	weights.reserve(weightedEdges.size());

	for (const auto& [edge, weight] : weightedEdges)
	{
		edges.push_back(edge);
		weights.push_back(weight);
	}
	ToGraphizFormat(edges, weights);
}

int main()
{
	std::vector<Edge> edges;
	std::vector<EdgeWeightType> weights;

	size_t firstVertex;
	size_t secondVertex;
	EdgeWeightType weight;
	while ((std::cin >> firstVertex) && (std::cin >> secondVertex) && (std::cin >> weight))
	{
		edges.emplace_back(firstVertex, secondVertex);
		weights.push_back(weight);
	}

	ToGraphizFormat(edges, weights);
}
