#ifndef UNIONFIND_H
#define UNIONFIND_H

#include <vector>

class UnionFind
{
	std::vector<int> m_parent;
	std::vector<int> m_rank;

public:
	explicit UnionFind(int size);

	int find(int value);
	void unite(int first, int second);
};

#endif
