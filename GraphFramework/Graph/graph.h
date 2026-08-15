#ifndef GRAPH_H
#define GRAPH_H

#include "../Edge/edge.h"
#include <vector>
#include <map>
#include <string>
#include <utility>

using Matrix = std::vector<std::vector<int>>;

class Graph
{
    std::vector<Node*> m_nodes;
    std::vector<Edge> m_edges;
    bool m_oriented=false;
    Matrix m_adjacencyMatrix;
    Matrix m_costMatrix;

    std::vector<std::vector<int>> m_adjacencyList;

    std::map<std::pair<int, int>, Node*> m_coordToNode;
    Node* m_start = nullptr;
    std::vector<Node*> m_exitNodes;
    std::vector<int> m_currentPath;
    std::vector<int> m_parent;
    std::vector<int> m_distance;
    std::vector<int> m_topologicalOrder;

    static const int cellSize = 50;

    void printAdjacencyMatrix() const;

public:
    Graph();
    Graph(const Graph& other);
    Graph& operator=(const Graph& other);
    ~Graph();

    void addNode(QPoint p);
    void addNode(int row, int column, int value);
    void addEdge(Node* first, Node* second);
    void setEdgeCost(Node* first, Node* second, int cost);
    int getEdgeCost(Node* first, Node* second) const;
    void changeState();

    void readLabyrinth(Matrix& matrix, const std::string& filename);
    void constructLabyrinth(const Matrix& matrix);
    void breadthFirstSearch();
    void depthFirstSearch();
    bool hasCycle();
    void topologicalSort();
    void findShortestPaths(Node* source);
    void findPath(Node* exit);

    void resetNodeColors();
    void clearResources();

    std::vector<Node*> getNodes() const;
    std::vector<Edge> getEdges() const;
    Matrix getAdjacencyMatrix() const;
    std::vector<Node*> getExitNodes() const;
    std::vector<int> getCurrentPath() const;
    std::vector<int> getDistance() const;
    std::vector<int> getTopologicalOrder() const;

    bool isOriented() const;
};

#endif
