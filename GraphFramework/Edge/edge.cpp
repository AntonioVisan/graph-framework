#include "../Edge/edge.h"

Edge::Edge() 
    : m_first(nullptr), 
      m_second(nullptr),
      m_cost(0)
{}

Edge::Edge(Node* f, Node* s, int cost) 
    : m_first(f), 
      m_second(s), 
      m_cost(cost)
{}

bool Edge::equals(const Edge& other, bool oriented) const
{
    if (oriented)
        return m_first->getIndex() == other.getFirst()->getIndex() &&
        m_second->getIndex() == other.getSecond()->getIndex();

    return (m_first->getIndex() == other.getSecond()->getIndex() &&
        m_second->getIndex() == other.getFirst()->getIndex())
        ||
        (m_first->getIndex() == other.getFirst()->getIndex() &&
            m_second->getIndex() == other.getSecond()->getIndex());
}

void Edge::setCost(int cost)
{
    m_cost = cost;
}

Node* Edge::getFirst() const
{
    return m_first;
}

Node* Edge::getSecond() const
{
    return m_second;
}

int Edge::getCost() const
{
    return m_cost;
}