#include "flowedge.h"

FlowEdge::FlowEdge(int from, int to, int capacity, int reverseIndex) :
    m_from(from), 
    m_to(to), 
    m_capacity(capacity), 
    m_flow(0), 
    m_reverseIndex(reverseIndex)
{

}

int FlowEdge::GetFrom() const
{
    return m_from;
}

int FlowEdge::GetTo() const
{
    return m_to;
}

int FlowEdge::GetCapacity() const
{
    return m_capacity;
}

int FlowEdge::GetFlow() const
{
    return m_flow;
}

int FlowEdge::GetResidualCapacity() const
{
    return m_capacity - m_flow;
}

int FlowEdge::GetReverseIndex() const
{
    return m_reverseIndex;
}

void FlowEdge::AddFlow(int value)
{
    m_flow += value;
}

void FlowEdge::ResetFlow()
{
    m_flow = 0;
}