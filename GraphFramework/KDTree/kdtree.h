#ifndef KDTREE_H
#define KDTREE_H

#include <vector>

struct KDNode
{
	int m_ID;
	double m_x;
	double m_y;
	KDNode* m_left;
	KDNode* m_right;

	KDNode(int ID, double x, double y)
		: m_ID(ID), m_x(x), m_y(y), 
		  m_left(nullptr), m_right(nullptr) {}
};

class KDTree
{
public:
	KDTree(const std::vector<KDNode>& points);
	~KDTree();

	int FindNearest(double x, double y) const;

private:
	KDNode* BuildTree(std::vector<KDNode> points, int depth);
	
	void NearestNode(
		KDNode* node, 
		double x, 
		double y, 
		int depth, 
		KDNode*& bestNode, 
		double& bestDistance
	) const;

	void FreeTree(KDNode* node);

private:
	KDNode* m_root;
};

#endif
