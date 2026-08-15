#include "../Graph/graph.h"
#include <fstream>
#include <queue>
#include <stack>
#include <limits>
#include <algorithm>

void Graph::printAdjacencyMatrix() const
{
	std::ofstream fout("adjacencyMatrix.txt");

	fout << m_nodes.size() << std::endl;

	for (const auto& row : m_adjacencyMatrix)
	{
		for (int value : row)
			fout << value << " ";

		fout << std::endl;
	}
}

Graph::Graph() = default;

Graph::Graph(const Graph& other)
{
	m_oriented = other.m_oriented;
	m_adjacencyMatrix = other.m_adjacencyMatrix;
	m_costMatrix = other.m_costMatrix;
	m_adjacencyList = other.m_adjacencyList;
	m_currentPath = other.m_currentPath;
	m_parent = other.m_parent;
	m_distance = other.m_distance;
	m_topologicalOrder = other.m_topologicalOrder;

	for (const auto* node : other.m_nodes)
	{
		Node* newNode = new Node(node->getIndex(), node->getCoord());

		newNode->setRow(node->getRow());
		newNode->setColumn(node->getColumn());
		newNode->setColor(node->getColor());

		m_nodes.push_back(newNode);
		m_coordToNode[{node->getRow(), node->getColumn()}] = newNode;
	}

	for (const auto& edge : other.m_edges)
	{
		Node* firstNode = nullptr;
		Node* secondNode = nullptr;

		for (auto* node : m_nodes)
		{
			if (node->getIndex() == edge.getFirst()->getIndex())
				firstNode = node;

			if (node->getIndex() == edge.getSecond()->getIndex())
				secondNode = node;
		}

		m_edges.emplace_back(firstNode, secondNode, edge.getCost());
	}

	if (other.m_start)
	{
		for (auto* node : m_nodes)
		{
			if (node->getIndex() == other.m_start->getIndex())
			{
				m_start = node;
				break;
			}
		}
	}

	for (auto* exit : other.m_exitNodes)
	{
		for (auto* node : m_nodes)
		{
			if (node->getIndex() == exit->getIndex())
			{
				m_exitNodes.push_back(node);
				break;
			}
		}
	}
}

Graph& Graph::operator=(const Graph& other)
{
	if (this == &other)
		return *this;

	clearResources();

	m_oriented = other.m_oriented;
	m_adjacencyMatrix = other.m_adjacencyMatrix;
	m_costMatrix = other.m_costMatrix;
	m_adjacencyList = other.m_adjacencyList;
	m_currentPath = other.m_currentPath;
	m_parent = other.m_parent;
	m_distance = other.m_distance;
	m_topologicalOrder = other.m_topologicalOrder;

	for (const auto* node : other.m_nodes)
	{
		Node* newNode = new Node(node->getIndex(), node->getCoord());

		newNode->setRow(node->getRow());
		newNode->setColumn(node->getColumn());
		newNode->setColor(node->getColor());

		m_nodes.push_back(newNode);
		m_coordToNode[{node->getRow(), node->getColumn()}] = newNode;
	}

	for (const auto& edge : other.m_edges)
	{
		Node* firstNode = nullptr;
		Node* secondNode = nullptr;

		for (auto* node : m_nodes)
		{
			if (node->getIndex() == edge.getFirst()->getIndex())
				firstNode = node;

			if (node->getIndex() == edge.getSecond()->getIndex())
				secondNode = node;
		}

		m_edges.emplace_back(firstNode, secondNode, edge.getCost());
	}

	if (other.m_start)
	{
		for (auto* node : m_nodes)
		{
			if (node->getIndex() == other.m_start->getIndex())
			{
				m_start = node;
				break;
			}
		}
	}

	for (auto* exit : other.m_exitNodes)
	{
		for (auto* node : m_nodes)
		{
			if (node->getIndex() == exit->getIndex())
			{
				m_exitNodes.push_back(node);
				break;
			}
		}
	}

	return *this;
}

Graph::~Graph()
{
	clearResources();
}

void Graph::addNode(QPoint p)
{
	int index = static_cast<int>(m_nodes.size());
	Node* node = new Node(index, p);

	m_nodes.push_back(node);

	m_adjacencyMatrix.resize(m_nodes.size());
	m_costMatrix.resize(m_nodes.size());

	for (auto& row : m_adjacencyMatrix)
		row.resize(m_nodes.size(), 0);

	for (auto& row : m_costMatrix)
		row.resize(m_nodes.size(), 0);

	m_adjacencyList.resize(m_nodes.size());

	printAdjacencyMatrix();
}

void Graph::addNode(int row, int column, int value)
{
	int index = static_cast<int>(m_nodes.size());

	Node* node = new Node(
		index,
		QPoint(column * cellSize, row * cellSize)
	);

	node->setGridPos(row, column);

	m_nodes.push_back(node);
	m_coordToNode[{row, column}] = node;

	if (value == 3)
	{
		m_start = node;
		node->setColor(Qt::blue);
	}
	else if (value == 2)
	{
		m_exitNodes.push_back(node);
		node->setColor(Qt::red);
	}
	else
	{
		node->setColor(Qt::white);
	}
}

void Graph::addEdge(Node* first, Node* second)
{
	if (!first || !second)
		return;

	Edge edge(first, second);

	for (const auto& existingEdge : m_edges)
	{
		if (edge.equals(existingEdge, m_oriented))
			return;
	}

	int firstIndex = first->getIndex();
	int secondIndex = second->getIndex();

	if (firstIndex < 0 || secondIndex < 0)
		return;

	if (firstIndex >= static_cast<int>(m_adjacencyMatrix.size()) ||
		secondIndex >= static_cast<int>(m_adjacencyMatrix.size()))
	{
		return;
	}

	if (m_oriented)
	{
		m_adjacencyMatrix[firstIndex][secondIndex] = 1;
		m_adjacencyList[firstIndex].push_back(secondIndex);
	}
	else
	{
		m_adjacencyMatrix[firstIndex][secondIndex] = 1;
		m_adjacencyMatrix[secondIndex][firstIndex] = 1;

		m_adjacencyList[firstIndex].push_back(secondIndex);
		m_adjacencyList[secondIndex].push_back(firstIndex);
	}

	m_edges.push_back(edge);

	printAdjacencyMatrix();
}

void Graph::setEdgeCost(Node* first, Node* second, int cost)
{
	if (!first || !second)
		return;

	int firstIndex = first->getIndex();
	int secondIndex = second->getIndex();

	if (firstIndex < 0 || secondIndex < 0)
		return;

	if (firstIndex >= static_cast<int>(m_costMatrix.size()) ||
		secondIndex >= static_cast<int>(m_costMatrix.size()))
	{
		return;
	}

	for (auto& edge : m_edges)
	{
		if (edge.getFirst() == first &&
			edge.getSecond() == second)
		{
			edge.setCost(cost);
			m_costMatrix[firstIndex][secondIndex] = cost;
			return;
		}
	}
}

int Graph::getEdgeCost(Node* first, Node* second) const
{
	if (!first || !second)
		return 0;

	int firstIndex = first->getIndex();
	int secondIndex = second->getIndex();

	if (firstIndex < 0 || secondIndex < 0)
		return 0;

	if (firstIndex >= static_cast<int>(m_costMatrix.size()) ||
		secondIndex >= static_cast<int>(m_costMatrix.size()))
	{
		return 0;
	}

	return m_costMatrix[firstIndex][secondIndex];
}

void Graph::changeState()
{
	m_oriented = !m_oriented;

	if (!m_oriented)
	{
		std::vector<Edge> uniqueEdges;

		for (const auto& edge : m_edges)
		{
			bool exists = false;

			for (const auto& uniqueEdge : uniqueEdges)
			{
				if (edge.equals(uniqueEdge, false))
				{
					exists = true;
					break;
				}
			}

			if (!exists)
				uniqueEdges.push_back(edge);
		}

		m_edges = uniqueEdges;
	}

	m_adjacencyMatrix.assign(
		m_nodes.size(),
		std::vector<int>(m_nodes.size(), 0)
	);

	m_costMatrix.assign(
		m_nodes.size(),
		std::vector<int>(m_nodes.size(), 0)
	);

	m_adjacencyList.assign(m_nodes.size(), {});

	for (const auto& edge : m_edges)
	{
		int first = edge.getFirst()->getIndex();
		int second = edge.getSecond()->getIndex();

		m_adjacencyMatrix[first][second] = 1;
		m_adjacencyList[first].push_back(second);
		m_costMatrix[first][second] = edge.getCost();

		if (!m_oriented)
		{
			m_adjacencyMatrix[second][first] = 1;
			m_adjacencyList[second].push_back(first);
			m_costMatrix[second][first] = edge.getCost();
		}
	}

	printAdjacencyMatrix();
}

void Graph::readLabyrinth(Matrix& matrix, const std::string& filename)
{
	std::ifstream fin(filename);

	if (!fin)
		return;

	matrix.clear();

	std::string line;

	while (std::getline(fin, line))
	{
		std::vector<int> row;

		for (char character : line)
		{
			if (character >= '0' && character <= '3')
				row.push_back(character - '0');
		}

		if (!row.empty())
			matrix.push_back(row);
	}
}

void Graph::constructLabyrinth(const Matrix& matrix)
{
	clearResources();

	m_oriented = false;

	for (int row = 0; row < static_cast<int>(matrix.size()); ++row)
	{
		for (int column = 0; 
			column < static_cast<int>(matrix[row].size()); 
			++column)
		{
			if (matrix[row][column] != 0)
				addNode(row, column, matrix[row][column]);
		}
	}

	m_adjacencyMatrix.assign(
		m_nodes.size(),
		std::vector<int>(m_nodes.size(), 0)
	);

	m_costMatrix.assign(
		m_nodes.size(),
		std::vector<int>(m_nodes.size(), 0)
	);

	m_adjacencyList.assign(m_nodes.size(), {});

	for (int row = 0; row < static_cast<int>(matrix.size()); ++row)
	{
		for (int column = 0; 
			column < static_cast<int>(matrix[row].size()); 
			++column)
		{
			if (matrix[row][column] == 0)
				continue;

			Node* current = m_coordToNode[{row, column}];

			if (column + 1 < static_cast<int>(matrix[row].size()) &&
				matrix[row][column + 1] != 0)
			{
				Node* neighbour = 
					m_coordToNode[{row, column + 1}];

				addEdge(current, neighbour);
			}

			if (row + 1 < static_cast<int>(matrix.size()) &&
				column < static_cast<int>(matrix[row + 1].size()) &&
				matrix[row + 1][column] != 0)
			{
				Node* neighbour 
					= m_coordToNode[{row + 1, column}];

				addEdge(current, neighbour);
			}
		}
	}
}

void Graph::breadthFirstSearch()
{
	if (!m_start)
		return;

	m_parent.assign(m_nodes.size(), -1);

	std::vector<bool> visited(m_nodes.size(), false);
	std::queue<int> queue;

	int startIndex = m_start->getIndex();

	visited[startIndex] = true;
	queue.push(startIndex);

	while (!queue.empty())
	{
		int current = queue.front();
		queue.pop();

		for (int neighbour : m_adjacencyList[current])
		{
			if (!visited[neighbour])
			{
				visited[neighbour] = true;
				m_parent[neighbour] = current;
				queue.push(neighbour);
			}
		}
	}
}

void Graph::depthFirstSearch()
{
	m_topologicalOrder.clear();

	int n = static_cast<int>(m_nodes.size());

	if (n == 0)
		return;

	std::vector<int> state(n, 0);

	for (int start = 0; start < n; ++start)
	{
		if (state[start] != 0)
			continue;

		std::stack<std::pair<int, int>> stack;

		state[start] = 1;
		stack.push({ start, 0 });

		while (!stack.empty())
		{
			int current = stack.top().first;
			int& neighbourIndex = stack.top().second;

			if (neighbourIndex <
				static_cast<int>(m_adjacencyList[current].size()))
			{
				int neighbour =
					m_adjacencyList[current][neighbourIndex++];

				if (state[neighbour] == 0)
				{
					state[neighbour] = 1;
					stack.push({ neighbour, 0 });
				}
			}
			else
			{
				state[current] = 2;
				m_topologicalOrder.push_back(current);
				stack.pop();
			}
		}
	}

	std::reverse(
		m_topologicalOrder.begin(),
		m_topologicalOrder.end()
	);
}

bool Graph::hasCycle()
{
	int n = static_cast<int>(m_nodes.size());

	std::vector<int> state(n, 0);

	for (int start = 0; start < n; ++start)
	{
		if (state[start] != 0)
			continue;

		std::stack<std::pair<int, int>> stack;

		state[start] = 1;
		stack.push({ start, 0 });

		while (!stack.empty())
		{
			int current = stack.top().first;
			int& neighbourIndex = stack.top().second;

			if (neighbourIndex >=
				static_cast<int>(m_adjacencyList[current].size()))
			{
				state[current] = 2;
				stack.pop();
				continue;
			}

			int neighbour =
				m_adjacencyList[current][neighbourIndex++];

			if (state[neighbour] == 1)
				return true;

			if (state[neighbour] == 0)
			{
				state[neighbour] = 1;
				stack.push({ neighbour, 0 });
			}
		}
	}

	return false;
}

void Graph::topologicalSort()
{
	m_topologicalOrder.clear();

	if (hasCycle())
		return;

	depthFirstSearch();
}

void Graph::findShortestPaths(Node* source)
{
	m_distance.assign(
		m_nodes.size(),
		std::numeric_limits<int>::max()
	);

	m_parent.assign(m_nodes.size(), -1);
	m_currentPath.clear();

	if (!source)
		return;

	int sourceIndex = source->getIndex();

	if (sourceIndex < 0 ||
		sourceIndex >= static_cast<int>(m_nodes.size()))
	{
		return;
	}

	topologicalSort();

	if (m_topologicalOrder.empty())
		return;

	m_distance[sourceIndex] = 0;

	auto sourcePosition =
		std::find(
			m_topologicalOrder.begin(),
			m_topologicalOrder.end(),
			sourceIndex
		);

	if (sourcePosition == m_topologicalOrder.end())
		return;

	for (auto it = sourcePosition;
		it != m_topologicalOrder.end();
		++it)
	{
		int current = *it;

		if (m_distance[current] ==
			std::numeric_limits<int>::max())
		{
			continue;
		}

		for (int neighbour : m_adjacencyList[current])
		{
			int cost = m_costMatrix[current][neighbour];

			if (m_distance[current] + cost <
				m_distance[neighbour])
			{
				m_distance[neighbour] =
					m_distance[current] + cost;

				m_parent[neighbour] = current;
			}
		}
	}
}

void Graph::findPath(Node* exit)
{
	if (!exit || m_parent.empty())
		return;

	m_currentPath.clear();

	int current = exit->getIndex();

	if (current < 0 || 
		current >= static_cast<int>(m_parent.size()))
	{
		return;
	}

	if (m_distance.empty())
	{
		if (!m_start)
			return;

		if (current != m_start->getIndex() &&
			m_parent[current] == -1)
		{
			return;
		}
	}
	else if (m_distance[current] ==
		std::numeric_limits<int>::max())
	{
		return;
	}

	while (current != -1)
	{
		m_currentPath.push_back(current);

		if (!m_start || m_nodes[current] != m_start)
			m_nodes[current]->setColor(Qt::green);

		current = m_parent[current];
	}

	std::reverse(m_currentPath.begin(), m_currentPath.end());
}

void Graph::resetNodeColors()
{
	for (auto* node : m_nodes)
	{
		if (node == m_start)
			node->setColor(Qt::blue);
		else
		{
			bool isExit = false;

			for (auto* exit : m_exitNodes)
			{
				if (node == exit)
				{
					isExit = true;
					break;
				}
			}

			if (isExit)
				node->setColor(Qt::red);
			else
				node->setColor(Qt::white);
		}
	}

	m_currentPath.clear();
}

void Graph::clearResources()
{
	for (auto* node : m_nodes)
		delete node;

	m_nodes.clear();
	m_edges.clear();
	m_adjacencyMatrix.clear();
	m_costMatrix.clear();
	m_adjacencyList.clear();
	m_coordToNode.clear();
	m_start = nullptr;
	m_exitNodes.clear();
	m_currentPath.clear();
	m_parent.clear();
	m_distance.clear();
	m_topologicalOrder.clear();
}

std::vector<Node*> Graph::getNodes() const
{
	return m_nodes;
}

std::vector<Edge> Graph::getEdges() const
{
	return m_edges;
}

Matrix Graph::getAdjacencyMatrix() const
{
	return m_adjacencyMatrix;
}

std::vector<Node*> Graph::getExitNodes() const
{
	return m_exitNodes;
}

std::vector<int> Graph::getCurrentPath() const
{
	return m_currentPath;
}

std::vector<int> Graph::getDistance() const
{
	return m_distance;
}

std::vector<int> Graph::getTopologicalOrder() const
{
	return m_topologicalOrder;
}

bool Graph::isOriented() const
{
	return m_oriented;
}