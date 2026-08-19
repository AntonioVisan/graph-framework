#include "flownetwork.h"

#include <algorithm>
#include <climits>
#include <queue>

FlowNetwork::FlowNetwork(int nrNodes) :
	m_nrNodes(nrNodes),
	m_currentFlow(0),
	m_adjacencyList(nrNodes)
{

}

void FlowNetwork::AddNode()
{
	m_adjacencyList.emplace_back();
	m_nrNodes++;
}

int FlowNetwork::AddEdge(int from, int to, int capacity)
{
	int reverseIndexForward =
		static_cast<int>(m_adjacencyList[to].size());
	int reverseIndexBackward =
		static_cast<int>(m_adjacencyList[from].size());

	m_adjacencyList[from].emplace_back(
		from,
		to,
		capacity,
		reverseIndexForward
	);

	m_adjacencyList[to].emplace_back(
		to,
		from,
		0,
		reverseIndexBackward
	);

	return reverseIndexBackward;
}

void FlowNetwork::Initialize()
{
	m_currentFlow = 0;
	m_lastPath.clear();

	for (auto& edges : m_adjacencyList)
	{
		for (auto& edge : edges)
		{
			edge.ResetFlow();
		}
	}
}

bool FlowNetwork::BFS(
	int source,
	int target,
	std::vector<int>& parentNode,
	std::vector<int>& parentEdge
)
{
	parentNode.assign(m_nrNodes, -1);
	parentEdge.assign(m_nrNodes, -1);

	std::queue<int> queue;

	queue.push(source);
	parentNode[source] = source;

	while (!queue.empty())
	{
		int currentNode = queue.front();
		queue.pop();

		for (int j = 0;
			j < static_cast<int>(m_adjacencyList[currentNode].size());
			j++)
		{
			const FlowEdge& currentEdge =
				m_adjacencyList[currentNode][j];

			if (
				parentNode[currentEdge.GetTo()] == -1 &&
				currentEdge.GetResidualCapacity() > 0
				)
			{
				parentNode[currentEdge.GetTo()] = currentNode;
				parentEdge[currentEdge.GetTo()] = j;

				if (currentEdge.GetTo() == target)
					return true;

				queue.push(currentEdge.GetTo());
			}
		}
	}

	return false;
}

bool FlowNetwork::Augment(int source, int target, bool storePath)
{
	std::vector<int> parentNode(m_nrNodes);
	std::vector<int> parentEdge(m_nrNodes);

	if (!BFS(source, target, parentNode, parentEdge))
		return false;

	if (storePath)
	{
		ConstructLastPath(
			source,
			target,
			parentNode,
			parentEdge
		);
	}

	int pathFlow = INT_MAX;

	for (
		int currentNode = target;
		currentNode != source;
		currentNode = parentNode[currentNode]
		)
	{
		int parent = parentNode[currentNode];
		int parentEdgeIndex = parentEdge[currentNode];

		const FlowEdge& currentEdge =
			m_adjacencyList[parent][parentEdgeIndex];

		pathFlow = std::min(
			pathFlow,
			currentEdge.GetResidualCapacity()
		);
	}

	for (
		int currentNode = target;
		currentNode != source;
		currentNode = parentNode[currentNode]
		)
	{
		int parent = parentNode[currentNode];
		int parentEdgeIndex = parentEdge[currentNode];

		FlowEdge& currentEdge =
			m_adjacencyList[parent][parentEdgeIndex];
		FlowEdge& backwardEdge =
			m_adjacencyList[currentNode]
			[currentEdge.GetReverseIndex()];

		currentEdge.AddFlow(pathFlow);
		backwardEdge.AddFlow(-pathFlow);
	}

	m_currentFlow += pathFlow;

	return true;
}

bool FlowNetwork::Step(int source, int target)
{
	return Augment(source, target, true);
}

int FlowNetwork::FordFulkerson(int source, int target)
{
	int maxFlow = 0;
	m_currentFlow = 0;

	std::vector<int> parentNode(m_nrNodes, -1);
	std::vector<int> parentEdge(m_nrNodes, -1);

	while (BFS(source, target, parentNode, parentEdge))
	{
		int pathFlow = INT_MAX;

		for (
			int currentNode = target;
			currentNode != source;
			currentNode = parentNode[currentNode]
			)
		{
			int parent = parentNode[currentNode];
			int parentEdgeIndex = parentEdge[currentNode];

			const FlowEdge& currentEdge =
				m_adjacencyList[parent][parentEdgeIndex];

			pathFlow = std::min(
				pathFlow,
				currentEdge.GetResidualCapacity()
			);
		}

		for (
			int currentNode = target;
			currentNode != source;
			currentNode = parentNode[currentNode]
			)
		{
			int parent = parentNode[currentNode];
			int parentEdgeIndex = parentEdge[currentNode];

			FlowEdge& currentEdge =
				m_adjacencyList[parent][parentEdgeIndex];
			FlowEdge& backwardEdge =
				m_adjacencyList[currentNode]
				[currentEdge.GetReverseIndex()];

			currentEdge.AddFlow(pathFlow);
			backwardEdge.AddFlow(-pathFlow);
		}

		maxFlow += pathFlow;
	}

	m_currentFlow = maxFlow;

	return maxFlow;
}

void FlowNetwork::ConstructLastPath(
	int source,
	int target,
	const std::vector<int>& parentNode,
	const std::vector<int>& parentEdge
)
{
	m_lastPath.clear();

	int currentNode = target;

	while (currentNode != source)
	{
		int fromNodeIndex = parentNode[currentNode];
		int edgeIndex = parentEdge[currentNode];

		m_lastPath.push_back(
			{ fromNodeIndex, edgeIndex }
		);

		currentNode = parentNode[currentNode];
	}

	std::reverse(
		m_lastPath.begin(),
		m_lastPath.end()
	);
}

std::vector<bool> FlowNetwork::MinCut(int source) const
{
	std::vector<bool> visited(m_nrNodes, false);
	std::queue<int> queue;

	queue.push(source);
	visited[source] = true;

	while (!queue.empty())
	{
		int currentNode = queue.front();
		queue.pop();

		for (const FlowEdge& currentEdge :
			m_adjacencyList[currentNode])
		{
			if (
				!visited[currentEdge.GetTo()] &&
				currentEdge.GetResidualCapacity() > 0
				)
			{
				visited[currentEdge.GetTo()] = true;
				queue.push(currentEdge.GetTo());
			}
		}
	}

	return visited;
}

int FlowNetwork::GetCurrentFlow() const
{
	return m_currentFlow;
}

const FlowEdgeMatrix& FlowNetwork::getAdjacencyList() const
{
	return m_adjacencyList;
}

const std::vector<std::pair<int, int> >& FlowNetwork::getLastPath() const
{
	return m_lastPath;
}
