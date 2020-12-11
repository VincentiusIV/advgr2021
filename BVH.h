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
	BVH( Scene *scene ) : scene( scene ) {}
	
	void ConstructBVH();
	AABB CalculateBounds(int first, int count );
	void Subdivide( BVHNode &node );
	void Partition( BVHNode &node );
	bool Intersect( Ray &r, RayHit &hit );
	void FindSplitPlane();

	int poolPtr;
	BVHNode root;
	BVHNode *pool;
	Scene *scene;
};

