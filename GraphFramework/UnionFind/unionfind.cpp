#include "../UnionFind/unionfind.h"

UnionFind::UnionFind(int size)
	: m_parent(size),
	m_rank(size, 0)
{
	for (int i = 0; i < size; ++i)
		m_parent[i] = i;
}

int UnionFind::find(int value)
{
	if (m_parent[value] != value)
		m_parent[value] = find(m_parent[value]);

	return m_parent[value];
}

void UnionFind::unite(int first, int second)
{
	first = find(first);
	second = find(second);

	if (first == second)
		return;

	if (m_rank[first] < m_rank[second])
	{
		m_parent[first] = second;
	}
	else if (m_rank[first] > m_rank[second])
	{
		m_parent[second] = first;
	}
	else
	{
		m_parent[second] = first;
		++m_rank[first];
	}
}