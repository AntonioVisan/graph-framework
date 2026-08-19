#ifndef FLOWNETWORK_H
#define FLOWNETWORK_H

#include "../FlowEdge/flowedge.h"
#include <vector>

using FlowEdgeMatrix = std::vector<std::vector<FlowEdge>>;

class FlowNetwork
{
private:
	int m_nrNodes;
	int m_currentFlow;
	FlowEdgeMatrix m_adjacencyList;
	std::vector<std::pair<int, int>> m_lastPath;

public:
	explicit FlowNetwork(int nrNodes);

	void AddNode();
	int AddEdge(int from, int to, int capacity);

	void Initialize();

	bool BFS(
		int source,
		int target,
		std::vector<int>& parentNode,
		std::vector<int>& parentEdge
	);

	bool Augment(
		int source,
		int target,
		bool storePath
	);

	bool Step(int source, int target);

	int FordFulkerson(int source, int target);

	void ConstructLastPath(
		int source,
		int target,
		const std::vector<int>& parentNode,
		const std::vector<int>& parentEdge
	);

	std::vector<bool> MinCut(int source) const;

	int GetCurrentFlow() const;

	const FlowEdgeMatrix& getAdjacencyList() const;

	const std::vector<std::pair<int, int>>& getLastPath() const;
};

#endif
