#include "virtualedge.h"

VirtualEdge::VirtualEdge(
	Node* from,
	Node* to,
	int fromNodeIndex,
	int edgeIndex
) :
	m_from(from),
	m_to(to),
	m_fromNodeIndex(fromNodeIndex),
	m_edgeIndex(edgeIndex),
	m_color(Qt::black)
{

}

Node* VirtualEdge::GetFrom() const
{
	return m_from;
}

Node* VirtualEdge::GetTo() const
{
	return m_to;
}

int VirtualEdge::GetFromNodeIndex() const
{
	return m_fromNodeIndex;
}

int VirtualEdge::GetEdgeIndex() const
{
	return m_edgeIndex;
}

QColor VirtualEdge::GetColor() const
{
	return m_color;
}

void VirtualEdge::SetColor(const QColor& color)
{
	m_color = color;
}