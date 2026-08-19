#ifndef FLOWEDGE_H
#define FLOWEDGE_H

class FlowEdge
{
private:
	int m_from;
	int m_to;
	int m_capacity;
	int m_flow;
	int m_reverseIndex;

public:
	FlowEdge(int from, int to, int capacity, int reverseIndex);

	int GetFrom() const;
	int GetTo() const;
	int GetCapacity() const;
	int GetFlow() const;
	int GetResidualCapacity() const;
	int GetReverseIndex() const;

	void AddFlow(int value);
	void ResetFlow();
};

#endif
