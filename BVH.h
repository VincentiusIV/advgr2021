#pragma once

struct BVHNode
{
	AABB bounds;
	bool isLeaf;
	int left, right;
	int first, count;
};

class BVH
{
  public:
	BVH( Scene *scene ) : scene( scene ) {}
	
	void ConstructBVH();
	AABB CalculateBounds(int first, int count );
	void Subdivide( int nodeIdx );
	void SplitNode( int nodeIdx );
	bool Intersect( Ray &r, RayHit &hit, int &depth );
	bool IntersectRecursive( Ray &r, RayHit &hit, BVHNode &current, int &depth );

	int poolPtr;
	BVHNode* root;
	BVHNode *pool;
	Scene *scene;
};

