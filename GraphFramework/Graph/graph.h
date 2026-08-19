#ifndef GRAPH_H
#define GRAPH_H

#include "../Edge/edge.h"
#include <vector>
#include <map>
#include <string>
#include <utility>

using Matrix = std::vector<std::vector<int>>;
using CostMatrix = std::vector<std::vector<double>>;
using EdgeMatrix = std::vector<std::vector<Edge>>;

class Graph
{
	std::vector<Node*> m_nodes;
	std::vector<Edge> m_edges;
	bool m_oriented = false;

	Matrix m_adjacencyMatrix;
	CostMatrix m_costMatrix;

	std::vector<std::vector<int>> m_adjacencyList;

	// Map-specific adjacency list: neighbour + edge cost.
	std::vector<std::vector<std::pair<int, double>>> m_mapAdjacencyList;

	// True when the graph was loaded from an XML map.
	bool m_isMap = false;

	std::map<std::pair<int, int>, Node*> m_coordToNode;
	std::map<int, Node*> m_idToNode;

	Node* m_start = nullptr;
	std::vector<Node*> m_exitNodes;
	std::vector<int> m_currentPath;
	std::vector<int> m_parent;
	std::vector<double> m_distance;
	std::vector<int> m_topologicalOrder;

	// Connected components.
	std::vector<std::vector<int>> m_connectedComponents;

	// Strongly connected components.
	std::vector<std::vector<int>> m_stronglyConnectedComponents;
	std::vector<int> m_componentIndex;
	std::vector<std::pair<int, int>> m_componentEdges;

	// Theme 6 - complete graph and distance matrix.
	CostMatrix m_distanceMatrix;
	EdgeMatrix m_completeGraph;

	// Theme 6 - Minimum Spanning Tree.
	std::vector<std::vector<int>> m_MSTAdjacencyList;

	// Number of edges in the graph loaded from the input file.
	int m_initialEdgeCount = 0;

	static const int cellSize = 50;

	void printAdjacencyMatrix() const;

public:
	Graph();
	Graph(const Graph& other);
	Graph& operator=(const Graph& other);
	~Graph();

	// Graph construction.
	void addNode(QPoint p);
	void addNode(int row, int column, int value);
	void addNode(int id, double longitude, double latitude);
	void addNode(
		const std::string& name,
		double latitude,
		double longitude
	);

	void addEdge(Node* first, Node* second);
	void addEdge(
		int from,
		int to,
		double length
	);

	void setEdgeCost(Node* first, Node* second, double cost);
	double getEdgeCost(Node* first, Node* second) const;

	void changeState();
	void loadFromXML(const std::string& filename);

	// Theme 6 - input and graph algorithms.
	void loadFromTXT(const std::string& filename);
	bool isConnected();
	void initDistanceMatrix();
	void floydWarshall();
	void buildCompleteGraphFromDistances();
	void buildMSTAdjacencyList(
		const std::vector<Edge>& mst,
		int nrNodes
	);
	std::vector<Edge> kruskal();
	void preorderTraverse(
		int currentNode,
		int parent,
		std::vector<int>& path
	);
	std::vector<int> travelingSalesmanProblem(
		const std::vector<Edge>& mst
	);

	// Labyrinth.
	void readLabyrinth(
		Matrix& matrix,
		const std::string& filename
	);
	void constructLabyrinth(const Matrix& matrix);
	void breadthFirstSearch();
	void depthFirstSearch();
	bool hasCycle();
	void topologicalSort();
	void findShortestPaths(Node* source);
	void dijkstra(Node* source, Node* target);
	void findPath(Node* exit);

	// Theme 4 - connected components.
	void findConnectedComponents();

	// Theme 4 - strongly connected components.
	void findStronglyConnectedComponents();

	// Reset.
	void resetNodeColors();
	void clearResources();

	// Getters.
	std::vector<Node*> getNodes() const;
	std::vector<Edge> getEdges() const;
	Matrix getAdjacencyMatrix() const;
	std::vector<Node*> getExitNodes() const;
	std::vector<int> getCurrentPath() const;
	std::vector<double> getDistance() const;
	std::vector<int> getTopologicalOrder() const;

	std::vector<std::vector<int>> getConnectedComponents() const;
	std::vector<std::vector<int>> getStronglyConnectedComponents() const;
	std::vector<std::pair<int, int>> getComponentEdges() const;

	// Theme 6 - getters.
	const CostMatrix& getDistanceMatrix() const;
	const EdgeMatrix& getCompleteGraph() const;
	int getInitialEdgeCount() const;

	bool isOriented() const;
};

#endif
