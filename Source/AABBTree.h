#ifndef __AABBTREE_H__
#define __AABBTREE_H__

#include "Globals.h"
#include "Geometry/AABB.h"

#include <memory>
#include <vector>
#include <map>
#include <forward_list>


#define AABB_NULL_NODE 0xffffffff
class GameObject;

struct AABBNode
{
	AABB aabb;
	GameObject* go;
	// tree links
	unsigned parentNodeIndex;
	unsigned leftNodeIndex;
	unsigned rightNodeIndex;
	// node linked list link
	unsigned nextNodeIndex;

	bool isLeaf() const { return leftNodeIndex == AABB_NULL_NODE; }

	AABBNode() : go(nullptr), parentNodeIndex(AABB_NULL_NODE), leftNodeIndex(AABB_NULL_NODE), rightNodeIndex(AABB_NULL_NODE), nextNodeIndex(AABB_NULL_NODE)
	{

	}
};

class AABBTree
{
private:
	std::map<GameObject*, unsigned> _objectNodeIndexMap;
	std::vector<AABBNode> _nodes;
	unsigned _rootNodeIndex;
	unsigned _allocatedNodeCount;
	unsigned _nextFreeNodeIndex;
	unsigned _nodeCapacity;
	unsigned _growthSize;

	unsigned allocateNode();
	void deallocateNode(unsigned nodeIndex);
	void insertLeaf(unsigned leafNodeIndex);
	void removeLeaf(unsigned leafNodeIndex);
	void updateLeaf(unsigned leafNodeIndex, const AABB& newAaab);
	void fixUpwardsTree(unsigned treeNodeIndex);


public:
	AABBTree(unsigned initialSize);
	~AABBTree();

	void insertObject(GameObject*);
	void removeObject(GameObject*);
	void updateObject(GameObject*);
	std::forward_list<GameObject*> queryOverlaps(const GameObject*) const;
	void DrawTree();
};

#endif // __AABBTREE_H__