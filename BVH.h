#pragma once

struct BVHNode
{
	AABB bounds;
	int left;
	int first, count;
	bool isLeaf;
	//vector<int> objIndices;
};

class BVH
{
  public:	
	BVH( int N ) : poolPtr( 1 ), N( N ), maxObjectsPerLeaf( 3 ), progressCounter( 0 )
	{
		nodeCount = N * 2 - 1;
		pool = new BVHNode[nodeCount];
		root = &pool[0];
	}
	~BVH() 
	{ 
		delete[] pool; 
	}
	void ConstructBVH();
	virtual AABB CalculateBounds( int first, int count ) = 0;
	void Subdivide( int nodeIdx, int maxNodeIdx );
	void SplitNodeBinary( int nodeIdx );
	void SplitNodeAt( int nodeIdx, int splitIndex );
	float SplitCost( int nodeCount, int leftFirst, int leftCount );
	float SmallestCostSAH( int nodeIdx, int &perfectSplit );
	float SmallestCostBin( int nodeIdx, int &perfectSplit );
	void SplitNodeSAH( int nodeIdx );
	void SplitNodeBin( int nodeIdx );
	bool Intersect( Ray &r, RayHit &hit );
	bool IntersectRecursive( Ray &r, RayHit &hit, int nodeIdx );
	virtual bool IntersectNode( BVHNode &node, Ray &r, RayHit &hit ) = 0;
	virtual vec3 GetPosition( int objIdx ) = 0; 

	int poolPtr, N, nodeCount, maxObjectsPerLeaf, progressCounter;
	
protected:
	BVHNode* root;
	BVHNode *pool;
	BVHNode &GetNode( int nodeIdx ) { return pool[nodeIdx]; }
	BVHNode &GetLeftNode( int nodeIdx ) { return pool[GetNode( nodeIdx ).left]; }
	BVHNode &GetRightNode( int nodeIdx ) { return pool[GetNode( nodeIdx ).left + 1]; }
};

