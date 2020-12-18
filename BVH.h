#pragma once

struct BVHNode
{
	AABB bounds = AABB();
	int left = 0;
	bool isLeaf = false;
	vector<uint> objIndices = vector<uint>();
	int count() { return objIndices.size(); }
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
	void SplitNodeSAH( int nodeIdx );
	void SplitNodeBin( int nodeIdx );
	void EnsureCorrectSize();
	bool Intersect( Ray &r, RayHit &hit );
	bool IntersectRecursive( Ray &r, RayHit &hit, int nodeIdx );
	virtual bool IntersectNode( BVHNode &node, Ray &r, RayHit &hit ) = 0;
	virtual vec3 GetObjectPosition( uint objIdx ) = 0; 
	virtual AABB GetObjectAABB( uint objIdx ) = 0;
	int poolPtr, N, nodeCount, maxObjectsPerLeaf, progressCounter;
	
protected:
	BVHNode* root;
	BVHNode *pool;
	BVHNode &GetNode( int nodeIdx ) { return pool[nodeIdx]; }
	BVHNode &GetLeftNode( int nodeIdx ) { return pool[GetNode( nodeIdx ).left]; }
	BVHNode &GetRightNode( int nodeIdx ) { return pool[GetNode( nodeIdx ).left + 1]; }
};

