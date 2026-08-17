#ifndef GRAPH_H
#define GRAPH_H

#include "../Edge/edge.h"
#include <vector>
#include <map>
#include <string>
#include <utility>

using Matrix = std::vector<std::vector<int>>;
using CostMatrix = std::vector<std::vector<double>>;

class Graph
{
    std::vector<Node*> m_nodes;
    std::vector<Edge> m_edges;
    bool m_oriented=false;

    Matrix m_adjacencyMatrix;
    CostMatrix m_costMatrix;

    std::vector<std::vector<int>> m_adjacencyList;

    std::map<std::pair<int, int>, Node*> m_coordToNode;
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
    void addEdge(Node* first, Node* second);
    void setEdgeCost(Node* first, Node* second, double cost);
    double getEdgeCost(Node* first, Node* second) const;
    void changeState();

    // Labyrinth.
    void readLabyrinth(Matrix& matrix, const std::string& filename);
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

    bool isOriented() const;
};

#endif
