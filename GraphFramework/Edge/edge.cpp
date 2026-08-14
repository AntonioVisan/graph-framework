#include "../Edge/edge.h"

Edge::Edge() : m_first(nullptr), m_second(nullptr){}

Edge::Edge(Node *f, Node *s) : m_first(f), m_second(s)
{

}

bool Edge::equals(const Edge &other, bool oriented) const
{
    if(oriented)
        return m_first->getIndex()==other.getFirst()->getIndex() && m_second->getIndex()==other.getSecond()->getIndex();
    return (m_first->getIndex()==other.getSecond()->getIndex() && m_second->getIndex()==other.getFirst()->getIndex())
            || (m_first->getIndex()==other.getFirst()->getIndex() && m_second->getIndex()==other.getSecond()->getIndex());
}

Node* Edge::getFirst() const
{
    return m_first;
}

Node* Edge::getSecond() const
{
    return m_second;
}
