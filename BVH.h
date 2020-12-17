#pragma once

struct BVHNode
{
	AABB bounds;
	int first, count;
};

class BVH
{
  public:	
	virtual void ConstructBVH();
	virtual AABB CalculateBounds( int first, int count ) = 0;
	void Subdivide( int nodeIdx );
	void SplitNode( int nodeIdx );
	void SplitNodeSAH( int nodeIdx );
	void SplitNodeBin( int nodeIdx );
	bool Intersect( Ray &r, RayHit &hit );
	bool IntersectRecursive( Ray &r, RayHit &hit, int nodeIdx );
	virtual bool IntersectNode( BVHNode &node, Ray &r, RayHit &hit ) = 0;
	virtual vec3 GetPosition( int objIdx ) = 0; 

	int N = 0, maxObjectsPerLeaf = 3;
	BVHNode* root;
	BVHNode *pool;
};

