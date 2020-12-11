#pragma once

struct BVHNode
{
	AABB bounds;
	int leftFirst;
	int count;

	int right() { return leftFirst + 1;  }
};

class BVH
{
  public:
	BVH( Scene *scene ) : scene( scene ) {}
	
	void ConstructBVH();
	AABB CalculateBounds(int first, int count );
	void Subdivide( BVHNode &node );
	void SplitNode( BVHNode &node );
	bool Intersect( Ray &r, RayHit &hit );

	int poolPtr;
	BVHNode root;
	BVHNode *pool;
	Scene *scene;
};

