#include "kdtree.h"
#include <algorithm>
#include <cmath>
#include <limits>

KDTree::KDTree(const std::vector<KDNode>& points)
{
	m_root = BuildTree(points, 0);
}

KDTree::~KDTree()
{
	FreeTree(m_root);
}

int KDTree::FindNearest(double x, double y) const
{
	KDNode* bestNode = nullptr;
	double bestDistance = std::numeric_limits<double>::max();

	NearestNode(
		m_root, 
		x, 
		y, 
		0, 
		bestNode, 
		bestDistance
	);

	if (bestNode)
		return bestNode->m_ID;
	
	return -1;
}

KDNode* KDTree::BuildTree(
	std::vector<KDNode> points, 
	int depth
)
{
	if (points.empty()) 
		return nullptr;

	int axis = depth % 2;

	auto comparator = [axis](const KDNode& a, const KDNode& b)
		{
			if (axis == 0)
				return a.m_x < b.m_x;

			return a.m_y < b.m_y;
		};

	std::sort(points.begin(), points.end(), comparator);

	int mid = static_cast<int>(points.size()) / 2;

	KDNode* node = new KDNode(points[mid]);

	std::vector<KDNode> left(
		points.begin(),
		points.begin() + mid
	);

	std::vector<KDNode> right(
		points.begin() + mid + 1,
		points.end()
	);

	node->m_left = BuildTree(left, depth + 1);
	node->m_right = BuildTree(right, depth + 1);

	return node;
}

void KDTree::NearestNode(
	KDNode* node, 
	double x, 
	double y, 
	int depth, 
	KDNode*& bestNode, 
	double& bestDistance
) const
{
	if (!node) 
		return;

	double distance = 
		std::hypot(node->m_x - x, node->m_y - y);

	if (distance < bestDistance)
	{
		bestDistance = distance;
		bestNode = node;
	}

	int axis = depth % 2;

	double nodeCoord;
	double pointCoord;

	if (axis == 0)
	{
		nodeCoord = node->m_x;
		pointCoord = x;
	}
	else
	{
		nodeCoord = node->m_y;
		pointCoord = y;
	}

	KDNode* next;
	KDNode* other;

	if (pointCoord < nodeCoord)
	{
		next = node->m_left;
		other = node->m_right;
	}
	else
	{
		next = node->m_right;
		other = node->m_left;
	}

	NearestNode(
		next, 
		x, 
		y, 
		depth + 1, 
		bestNode, 
		bestDistance
	);

	double delta;

	if (axis == 0)
		delta = std::abs(x - node->m_x);
	else
		delta = std::abs(y - node->m_y);

	if (delta < bestDistance)
	{
		NearestNode(
			other,
			x,
			y,
			depth + 1,
			bestNode,
			bestDistance
		);
	}
}

void KDTree::FreeTree(KDNode* node)
{
	if (!node) 
		return;

	FreeTree(node->m_left);
	FreeTree(node->m_right);

	delete node;
}