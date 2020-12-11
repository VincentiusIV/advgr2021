#pragma once

struct BVHNode
{
	AABB bounds;
	int leftFirst;
	int count;
};

class BVH
{
  public:
	BVH();
	
	void ConstructBVH( Scene *scene );
	AABB CalculateBounds( Scene *scene, int first, int count );
	void Subdivide( BVHNode &node );
	void Partition( BVHNode &node );
	bool Intersect( Ray &r, RayHit &hit );
	void FindSplitPlane();

	int poolPtr;
	BVHNode root;
	BVHNode *pool;
};

