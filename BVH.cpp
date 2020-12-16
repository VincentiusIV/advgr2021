#include "precomp.h"

void BVH::ConstructBVH()
{
	pool = new BVHNode[N * 2 - 1];
	root = &pool[0];
	poolPtr = 1;
	root->first = 0;
	root->count = N;
	root->bounds = CalculateBounds( root->first, root->count );
	Subdivide(0);
}

void BVH::Subdivide( int nodeIdx )
{
	BVHNode &node = pool[nodeIdx];
	if (node.count < maxObjectsPerLeaf)
	{
		return;
	}
	node.left = poolPtr++;
	node.right = poolPtr++;
	SplitNode(nodeIdx);
	Subdivide( node.left );
	Subdivide( node.right );
}

void BVH::SplitNode( int nodeIdx )
{
	BVHNode &node = pool[nodeIdx];
	BVHNode &left = pool[node.left];
	left.first = node.first;
	left.count = node.count / 2;
	left.bounds = CalculateBounds( left.first, left.count );

	BVHNode &right = pool[node.right];
	right.first = left.first+left.count;
	right.count = node.count - left.count;
	right.bounds = CalculateBounds( right.first, right.count );
}

bool BVH::IntersectRecursive( Ray &r, RayHit &hit, BVHNode &current )
{
	++r.bvhDepth;
	bool hitAnything = false;
	if ( current.count < maxObjectsPerLeaf )
	{
		hitAnything |= IntersectNode( current, r, hit );
	}
	else
	{
		if ( pool[current.left].bounds.Intersect( r ) )
			hitAnything |= IntersectRecursive( r, hit, pool[current.left] );
		if ( pool[current.right].bounds.Intersect( r ) )
			hitAnything |= IntersectRecursive( r, hit, pool[current.right] );
	}
	return hitAnything;
}

bool BVH::Intersect( Ray &r, RayHit &hit )
{
	return IntersectRecursive( r, hit, *root );

	// While method, doesnt work better than recursive cause of queue<int> allocation?
	//queue<int> open = queue<int>();
	//open.emplace( 0 );
	//bool hitAnything = false;

	//while ( open.size() > 0 )
	//{
	//	int currentIdx = open.front();
	//	open.pop();
	//	BVHNode &current = pool[currentIdx];

	//	if ( current.count < maxObjectsPerLeaf )
	//	{
	//		hitAnything |= IntersectNode( current, r, hit );
	//	}
	//	else
	//	{
	//		if ( pool[current.left].bounds.Intersect( r ) )
	//			open.push( current.left );
	//		if ( pool[current.right].bounds.Intersect( r ) )
	//			open.push( current.right );
	//	}
	//}
	//return hitAnything;
}
