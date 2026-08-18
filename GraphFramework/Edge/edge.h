#ifndef EDGE_H
#define EDGE_H

#include "../Node/node.h"

class Edge
{
	Node* m_first;
	Node* m_second;
	double m_cost;

public:
	Edge();
	Edge(Node* f, Node* s, double cost = 0);

	bool equals(const Edge& other, bool oriented) const;

	void setCost(double cost);

	Node* getFirst() const;
	Node* getSecond() const;
	double getCost() const;

	int getFrom() const;
	int getTo() const;
	double getLength() const;
};

#endif