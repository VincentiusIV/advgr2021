#pragma once

struct BVHNode
{
	AABB bounds;
	int left;
	int first, count;
	bool isLeaf;
};

class BVH
{
  public:	
	BVH( int N ) : N(N)
	{
		nodeCount = N * 2 - 1;
		pool = new BVHNode[nodeCount];
	}
	~BVH() 
	{ 
		delete[] pool; 
	}
	void ConstructBVH();
	virtual AABB CalculateBounds( int first, int count ) = 0;
	void Subdivide( int nodeIdx, int maxNodeIdx );
	void SplitNode( int nodeIdx );
	void SplitNodeSAH( int nodeIdx );
	void SplitNodeBin( int nodeIdx );
	bool Intersect( Ray &r, RayHit &hit );
	bool IntersectRecursive( Ray &r, RayHit &hit, int nodeIdx );
	virtual bool IntersectNode( BVHNode &node, Ray &r, RayHit &hit ) = 0;
	virtual vec3 GetPosition( int objIdx ) = 0; 

	int poolPtr, N, nodeCount, maxObjectsPerLeaf = 3, progressCounter;
	BVHNode* root;
	BVHNode *pool;
};

