#ifndef VIRTUALEDGE_H
#define VIRTUALEDGE_H

#include "../Node/node.h"
#include <QColor>

class VirtualEdge
{
private:
	Node* m_from;
	Node* m_to;
	int m_fromNodeIndex;
	int m_edgeIndex;
	QColor m_color;

public:
	VirtualEdge(
		Node* from,
		Node* to,
		int fromNodeIndex,
		int edgeIndex
	);

	Node* GetFrom() const;
	Node* GetTo() const;
	int GetFromNodeIndex() const;
	int GetEdgeIndex() const;
	QColor GetColor() const;

	void SetColor(const QColor& color);
};
#endif
