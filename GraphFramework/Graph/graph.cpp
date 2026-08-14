#include "../Graph/graph.h"
#include <fstream>

void Graph::printAdjacencyMatrix() const
{
	std::ofstream fout("adjacencyMatrix.txt");
	fout << m_nodes.size() << std::endl;
	for (size_t i = 0; i < m_adjacencyMatrix.size(); i++)
	{
		for (size_t j = 0; j < m_adjacencyMatrix[i].size(); j++)
			fout << m_adjacencyMatrix[i][j] << " ";

		fout << std::endl;
	}
}

Graph::Graph() = default;

Graph::Graph(const Graph& other)
{
	for (auto node : other.m_nodes)
	{
		int index = node->getIndex();
		QPoint p = node->getCoord();
		m_nodes.push_back(new Node(index, p));
	}
	for (const auto& edge : other.m_edges)
	{
		Node* firstNode = nullptr, * secondNode = nullptr;
		for (auto n : m_nodes)
		{
			if (n->getIndex() == edge.getFirst()->getIndex()) firstNode = n;
			else if (n->getIndex() == edge.getSecond()->getIndex()) secondNode = n;
		}
		m_edges.push_back(Edge(firstNode, secondNode));
	}
	m_oriented = other.m_oriented;
	m_adjacencyMatrix = other.m_adjacencyMatrix;
}

Graph& Graph::operator=(const Graph& other)
{
	if (this == &other) return *this;

	for (auto node : m_nodes) delete node;
	m_nodes.clear();
	m_edges.clear();

	for (const auto& node : other.m_nodes)
	{
		int index = node->getIndex();
		QPoint p = node->getCoord();
		m_nodes.push_back(new Node(index, p));
	}

	for (const auto& edge : other.m_edges)
	{
		Node* firstNode = nullptr, * secondNode = nullptr;
		for (auto n : m_nodes)
		{
			if (n->getIndex() == edge.getFirst()->getIndex()) firstNode = n;
			else if (n->getIndex() == edge.getSecond()->getIndex()) secondNode = n;
		}
		m_edges.push_back(Edge(firstNode, secondNode));
	}
	m_oriented = other.m_oriented;
	m_adjacencyMatrix = other.m_adjacencyMatrix;

	return *this;
}

Graph::~Graph()
{
	for (auto node : m_nodes)
		delete node;
}

void Graph::addNode(QPoint p)
{
	size_t nrNodes = m_nodes.size() + 1;
	Node* n = new Node(static_cast<int>(nrNodes), p);

	m_adjacencyMatrix.resize(nrNodes);
	for (auto& row : m_adjacencyMatrix)
		row.resize(nrNodes, 0);
	m_nodes.push_back(n);
	printAdjacencyMatrix();
}

void Graph::addEdge(Node* f, Node* s)
{
	Edge e(f, s);

	for (const auto& edge : m_edges)
	{
		if (e.equals(edge, m_oriented))
			return;
	}

	int i = f->getIndex();
	int j = s->getIndex();

	if (m_oriented)
		m_adjacencyMatrix[i - 1][j - 1] = 1;
	else 
		m_adjacencyMatrix[i - 1][j - 1] = m_adjacencyMatrix[j - 1][i - 1] = 1;

	m_edges.push_back(e);
	printAdjacencyMatrix();
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

			for (const auto& uEdge : uniqueEdges)
			{
				if (edge.equals(uEdge, false))
				{
					exists = true;
					break;
				}
			}

			if (!exists)
				uniqueEdges.push_back(edge);
		}

		m_edges = uniqueEdges;

		m_adjacencyMatrix.assign(m_nodes.size(), std::vector<int>(m_nodes.size(), 0));

		for (const auto& edge : m_edges)
		{
			int i = edge.getFirst()->getIndex();
			int j = edge.getSecond()->getIndex();

			m_adjacencyMatrix[i - 1][j - 1] = m_adjacencyMatrix[j - 1][i - 1] = 1;
		}

		printAdjacencyMatrix();
	}
	else
	{
		m_adjacencyMatrix.assign(m_nodes.size(), std::vector<int>(m_nodes.size(), 0));

		for (const auto& edge : m_edges)
		{
			int i = edge.getFirst()->getIndex();
			int j = edge.getSecond()->getIndex();

			m_adjacencyMatrix[i - 1][j - 1] = 1;
		}

		printAdjacencyMatrix();
	}
}

std::vector<Node*> Graph::getNodes() const
{
	return m_nodes;
}

std::vector<Edge> Graph::getEdges() const
{
	return m_edges;
}

std::vector<std::vector<int>> Graph::getAdjacencyMatrix() const
{
	return m_adjacencyMatrix;
}

bool Graph::isOriented() const
{
	return m_oriented;
}
