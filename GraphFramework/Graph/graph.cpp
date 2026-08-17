#include "../Graph/graph.h"
#include "../XML/rapidxml.hpp"
#include "../XML/rapidxml_utils.hpp"
#include <fstream>
#include <queue>
#include <stack>
#include <limits>
#include <algorithm>
#include <functional>

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
	m_connectedComponents = other.m_connectedComponents;
	m_stronglyConnectedComponents = other.m_stronglyConnectedComponents;
	m_componentIndex = other.m_componentIndex;
	m_componentEdges = other.m_componentEdges;

	for (const auto* node : other.m_nodes)
	{
		Node* newNode = new Node(node->getIndex(), node->getCoord());

		newNode->setRow(node->getRow());
		newNode->setColumn(node->getColumn());
		newNode->setColor(node->getColor());
		newNode->setGeoCoord(
			node->getLongitude(),
			node->getLatitude()
		);

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
	m_connectedComponents = other.m_connectedComponents;
	m_stronglyConnectedComponents = other.m_stronglyConnectedComponents;
	m_componentIndex = other.m_componentIndex;
	m_componentEdges = other.m_componentEdges;

	for (const auto* node : other.m_nodes)
	{
		Node* newNode = new Node(node->getIndex(), node->getCoord());

		newNode->setRow(node->getRow());
		newNode->setColumn(node->getColumn());
		newNode->setColor(node->getColor());
		newNode->setGeoCoord(
			node->getLongitude(),
			node->getLatitude()
		);

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
		row.resize(m_nodes.size(), 0.0);

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

void Graph::addNode(int id, double longitude, double latitude)
{
	Node* node = new Node(id, QPoint(0, 0));

	node->setGeoCoord(longitude, latitude);

	m_nodes.push_back(node);

	m_idToNode[id] = node;
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

void Graph::setEdgeCost(Node* first, Node* second, double cost)
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

double Graph::getEdgeCost(Node* first, Node* second) const
{
	if (!first || !second)
		return 0.0;

	int firstIndex = first->getIndex();
	int secondIndex = second->getIndex();

	if (firstIndex < 0 || secondIndex < 0)
		return 0.0;

	if (firstIndex >= static_cast<int>(m_costMatrix.size()) ||
		secondIndex >= static_cast<int>(m_costMatrix.size()))
	{
		return 0.0;
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
		std::vector<double>(m_nodes.size(), 0.0)
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

	m_connectedComponents.clear();
	m_stronglyConnectedComponents.clear();
	m_componentIndex.clear();
	m_componentEdges.clear();

	printAdjacencyMatrix();
}

void Graph::loadFromXML(const std::string& filename)
{
	clearResources();

	m_oriented = false;

	rapidxml::file<> xmlFile(filename.c_str());

	rapidxml::xml_document<> doc;
	doc.parse<0>(xmlFile.data());

	rapidxml::xml_node<>* root = doc.first_node("map");

	if (!root)
		return;

	rapidxml::xml_node<>* nodes = root->first_node("nodes");
	rapidxml::xml_node<>* arcs = root->first_node("arcs");

	if (!nodes || !arcs)
		return;

	// Load nodes.
	for (rapidxml::xml_node<>* node = nodes->first_node("node");
		node != nullptr;
		node = node->next_sibling("node"))
	{
		int id =
			std::stoi(node->first_attribute("id")->value());

		double longitude =
			std::stod(node->first_attribute("longitude")->value());

		double latitude =
			std::stod(node->first_attribute("latitude")->value());

		longitude /= 100000.0;
		latitude /= 100000.0;

		addNode(id, longitude, latitude);
	}

	// Initialize graph structures.
	m_adjacencyMatrix.assign(
		m_nodes.size(),
		std::vector<int>(m_nodes.size(), 0)
	);

	m_costMatrix.assign(
		m_nodes.size(),
		std::vector<double>(m_nodes.size(), 0.0)
	);

	m_adjacencyList.assign(m_nodes.size(), {});

	// Load arcs.
	for (rapidxml::xml_node<>* arc = arcs->first_node("arc");
		arc != nullptr;
		arc = arc->next_sibling("arc"))
	{
		int from =
			std::stoi(arc->first_attribute("from")->value());

		int to =
			std::stoi(arc->first_attribute("to")->value());

		double length =
			std::stod(arc->first_attribute("length")->value());

		auto firstIt = m_idToNode.find(from);
		auto secondIt = m_idToNode.find(to);

		if (firstIt == m_idToNode.end() ||
			secondIt == m_idToNode.end())
		{
			continue;
		}

		Node* first = firstIt->second;
		Node* second = secondIt->second;

		addEdge(first, second);
		setEdgeCost(first, second, length);
	}
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
		std::vector<double>(m_nodes.size(), 0.0)
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
		std::numeric_limits<double>::infinity()
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

	m_distance[sourceIndex] = 0.0;

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
			std::numeric_limits<double>::infinity())
		{
			continue;
		}

		for (int neighbour : m_adjacencyList[current])
		{
			double cost = m_costMatrix[current][neighbour];

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

void Graph::dijkstra(Node* source, Node* target)
{
	m_distance.assign(
		m_nodes.size(),
		std::numeric_limits<double>::infinity()
	);

	m_parent.assign(m_nodes.size(), -1);
	m_currentPath.clear();

	if (!source || !target)
		return;

	int sourceIndex = source->getIndex();
	int targetIndex = target->getIndex();

	if (sourceIndex < 0 ||
		targetIndex < 0 ||
		sourceIndex >= static_cast<int>(m_nodes.size()) ||
		targetIndex >= static_cast<int>(m_nodes.size()))
	{
		return;
	}

	std::vector<bool> visited(m_nodes.size(), false);

	std::priority_queue<
		std::pair<double, int>,
		std::vector<std::pair<double, int>>,
		std::greater<>
	> pq;

	m_distance[sourceIndex] = 0.0;

	pq.push({ 0.0, sourceIndex });

	while (!pq.empty())
	{
		auto [distanceNode, current] = pq.top();
		pq.pop();

		if (visited[current])
			continue;

		visited[current] = true;

		if (current == targetIndex)
			return;

		for (int neighbour : m_adjacencyList[current])
		{
			double cost = m_costMatrix[current][neighbour];

			if (!visited[neighbour] &&
				distanceNode + cost < m_distance[neighbour])
			{
				m_distance[neighbour] =
					distanceNode + cost;

				m_parent[neighbour] = current;

				pq.push({
					m_distance[neighbour],
					neighbour
					});
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
		std::numeric_limits<double>::infinity())
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

void Graph::findConnectedComponents()
{
	m_connectedComponents.clear();

	if (m_nodes.empty())
		return;

	std::vector<bool> visited(m_nodes.size(), false);

	const std::vector<QColor> colors =
	{
		Qt::red,
		Qt::green,
		Qt::blue,
		Qt::yellow,
		Qt::cyan,
		Qt::magenta,
		Qt::gray,
		Qt::darkRed,
		Qt::darkGreen,
		Qt::darkBlue,
		Qt::darkYellow,
		Qt::darkCyan,
		Qt::darkMagenta,
		Qt::darkGray,
		Qt::lightGray
	};

	int colorIndex = 0;

	for (int start = 0; start < static_cast<int>(m_nodes.size()); ++start)
	{
		if (visited[start])
			continue;

		std::vector<int> component;
		std::stack<int> stack;

		stack.push(start);
		visited[start] = true;

		while (!stack.empty())
		{
			int current = stack.top();
			stack.pop();

			component.push_back(current);

			for (int neighbour : m_adjacencyList[current])
			{
				if (!visited[neighbour])
				{
					visited[neighbour] = true;
					stack.push(neighbour);
				}
			}
		}

		m_connectedComponents.push_back(component);

		QColor componentColor =
			colors[colorIndex % colors.size()];

		for (int nodeIndex : component)
			m_nodes[nodeIndex]->setColor(componentColor);

		++colorIndex;
	}
}

void Graph::findStronglyConnectedComponents()
{
	m_stronglyConnectedComponents.clear();
	m_componentIndex.clear();
	m_componentEdges.clear();

	if (m_nodes.empty())
		return;

	const int n = static_cast<int>(m_nodes.size());

	// First DFS: determine finishing times.
	std::vector<bool> visited(n, false);
	std::vector<int> finishingOrder;

	std::function<void(int)> dfs = [&](int node)
		{
			visited[node] = true;

			for (int neighbour : m_adjacencyList[node])
			{
				if (!visited[neighbour])
					dfs(neighbour);
			}

			finishingOrder.push_back(node);
		};

	for (int i = 0; i < n; ++i)
	{
		if (!visited[i])
			dfs(i);
	}

	// Build the transposed adjacency list locally.
	Matrix transposed(n);

	for (int i = 0; i < n; ++i)
	{
		for (int neighbour : m_adjacencyList[i])
			transposed[neighbour].push_back(i);
	}

	// Second DFS on the transposed graph.
	std::fill(visited.begin(), visited.end(), false);

	m_componentIndex.assign(n, -1);

	std::function<void(int, int)> dfsTranspose =
		[&](int node, int componentIndex)
		{
			visited[node] = true;
			m_componentIndex[node] = componentIndex;

			m_stronglyConnectedComponents.back().push_back(node);

			for (int neighbour : transposed[node])
			{
				if (!visited[neighbour])
					dfsTranspose(neighbour, componentIndex);
			}
		};

	int componentIndex = 0;

	for (auto it = finishingOrder.rbegin();
		it != finishingOrder.rend();
		++it)
	{
		int start = *it;

		if (visited[start])
			continue;

		m_stronglyConnectedComponents.emplace_back();

		dfsTranspose(start, componentIndex);

		++componentIndex;
	}

	// Determine the arcs between strongly connected components.
	for (const auto& edge : m_edges)
	{
		int first = edge.getFirst()->getIndex();
		int second = edge.getSecond()->getIndex();

		int firstComponent = m_componentIndex[first];
		int secondComponent = m_componentIndex[second];

		if (firstComponent == secondComponent)
			continue;

		std::pair<int, int> componentEdge =
		{
			firstComponent,
			secondComponent
		};

		if (std::find(
			m_componentEdges.begin(),
			m_componentEdges.end(),
			componentEdge
		) == m_componentEdges.end())
		{
			m_componentEdges.push_back(componentEdge);
		}
	}
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
	m_idToNode.clear();

	m_start = nullptr;
	m_exitNodes.clear();

	m_currentPath.clear();
	m_parent.clear();
	m_distance.clear();
	m_topologicalOrder.clear();

	m_connectedComponents.clear();
	m_stronglyConnectedComponents.clear();
	m_componentIndex.clear();
	m_componentEdges.clear();
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

std::vector<double> Graph::getDistance() const
{
	return m_distance;
}

std::vector<int> Graph::getTopologicalOrder() const
{
	return m_topologicalOrder;
}

std::vector<std::vector<int>> Graph::getConnectedComponents() const
{
	return m_connectedComponents;
}

std::vector<std::vector<int>> Graph::getStronglyConnectedComponents() const
{
	return m_stronglyConnectedComponents;
}

std::vector<std::pair<int, int>> Graph::getComponentEdges() const
{
	return m_componentEdges;
}

bool Graph::isOriented() const
{
	return m_oriented;
}