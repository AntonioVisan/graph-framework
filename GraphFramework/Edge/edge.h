#ifndef EDGE_H
#define EDGE_H

#include "../Node/node.h"

class Edge
{
	Node* m_first;
	Node* m_second;
	int m_cost;

public:
	Edge();
	Edge(Node* f, Node* s, int cost = 0);

	bool equals(const Edge& other, bool oriented) const;

	void setCost(int cost);

	Node* getFirst() const;
	Node* getSecond() const;
	int getCost() const;
};

#endif