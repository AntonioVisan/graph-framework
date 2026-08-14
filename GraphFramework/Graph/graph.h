#ifndef GRAPH_H
#define GRAPH_H
#include "../Edge/edge.h"
#include <vector>
class Graph
{
    std::vector<Node*> m_nodes;
    std::vector<Edge> m_edges;
    bool m_oriented=false;
    std::vector<std::vector<int>> m_adjacencyMatrix;
    void printAdjacencyMatrix();
public:
    Graph();
    Graph(const Graph& other);
    Graph& operator=(const Graph& other);
    ~Graph();
    void addNode(QPoint p);
    void addEdge(Node* f, Node* s);
    void changeState();
    std::vector<Node*> getNodes();
    std::vector<Edge> getEdges();
    std::vector<std::vector<int>> getAdjacencyMatrix();
    bool isOriented();
};

#endif
