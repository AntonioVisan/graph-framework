#ifndef EDGE_H
#define EDGE_H
#include "../Node/node.h"
class Edge
{
    Node* m_first;
    Node* m_second;
public:
    Edge();
    Edge(Node* f, Node* s);
    bool equals(const Edge &other, bool oriented) const;
    Node* getFirst() const;
    Node* getSecond() const;

};

#endif
