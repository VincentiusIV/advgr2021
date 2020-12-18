#include "precomp.h"
#include <omp.h>

void BVH::ConstructBVH()
{
	std::string notifyString = "Started BVH Construction for N=" + to_string( N ) + " \n"; 
	printf( notifyString.c_str());
	timer t;
	t.reset();
	nodeCount = N * 2 - 1;

	if (nodeCount <= 0)
	{
		printf( "Error: Trying to construct a BVH for <= 0 objects..." );
		return;
	}

	for ( size_t i = 0; i < nodeCount; i++ )
	{
		pool[i].isLeaf = false;
	}

	root->first = 0;
	root->count = N;
	root->bounds = CalculateBounds( root->first, root->count );

	progressCounter = 0;
	Subdivide( 0, nodeCount );	

	string timeString = "Finished BVH Construction after t=" + to_string( t.elapsed() ) + "\n";
	printf( timeString.c_str());
}

void BVH::Subdivide( int nodeIdx, int maxNodeIdx )
{
	if ( nodeIdx >= maxNodeIdx )
		return;
	BVHNode &node = pool[nodeIdx];
	++progressCounter;
	std::cerr << "\rBVH subdivisions: " << double(progressCounter) << ' ' << std::flush;
	if ( node.isLeaf || node.count < maxObjectsPerLeaf )
	{
		node.isLeaf = true;
		return;
	}
	node.left = poolPtr;
	poolPtr += 2;
	SplitNodeSAH(nodeIdx);
	Subdivide( node.left, maxNodeIdx );
	Subdivide( node.left+1, maxNodeIdx );
}

void BVH::SplitNodeBinary( int nodeIdx )
{
	BVHNode &node = GetNode(nodeIdx);
	SplitNodeAt( nodeIdx, node.count / 2 );
}

void BVH::SplitNodeAt( int nodeIdx, int splitIndex )
{
	BVHNode &node = pool[nodeIdx];
	BVHNode &left = pool[node.left];
	left.first = node.first;
	left.count = splitIndex;
	left.bounds = CalculateBounds( left.first, left.count );
	BVHNode &right = pool[node.left + 1];
	right.first = left.first + left.count;
	right.count = node.count - left.count;
	right.bounds = CalculateBounds( right.first, right.count );
}

float BVH::SplitCost( int nodeCount, int leftFirst, int leftCount )
{
	int rigthFirst = leftFirst + leftCount;
	int rightCount = nodeCount - leftCount;
	AABB leftAABB = CalculateBounds( leftFirst, leftCount );
	AABB rightAABB = CalculateBounds( rigthFirst, rightCount );
	return ( leftAABB.Area() * leftCount ) + ( rightAABB.Area() * rightCount );
}

float BVH::SmallestCostSAH( int nodeIdx, int &bestSplit )
{
	BVHNode &node = GetNode( nodeIdx );
	BVHNode &left = GetLeftNode( nodeIdx );
	float areaNode = node.bounds.Area();
	float smallestCost = areaNode * node.count;
	for ( int i = 1; i < node.count; i++ )
	{
		std::cerr << "\rLarge SAH Subdivision: " << double( i ) / double( node.count ) * 100 << "%" << std::flush;
		float costSplit = SplitCost( node.count, node.first, i );
		if ( costSplit < bestSplit )
		{
			smallestCost = costSplit;
			bestSplit = i;
		}
	}
	return smallestCost;
}

void BVH::SplitNodeSAH(int nodeIdx)
{
	BVHNode &node = GetNode( nodeIdx );
	int bestSplit = node.count/2;
	SmallestCostSAH( node.first, bestSplit );		
	//if(smallestCost + 0.000001 >= costParent) 
	//{
	//	node.isLeaf = true;
	//	return;
	//}
	SplitNodeAt( nodeIdx, bestSplit );
}

float BVH::SmallestCostBin( int nodeIdx, int &perfectSplit )
{
	return 0.0f;
}

void BVH::SplitNodeBin(int nodeIdx)
{
	BVHNode &node = GetNode( nodeIdx );
	BVHNode &left = GetLeftNode( nodeIdx );
	BVHNode &right = GetRightNode( nodeIdx );

	vec3 size = node.bounds.max - node.bounds.min;
	float costParent = node.bounds.Area() * node.count;
	float smallestCost = costParent;
	float areaLeft;
	float areaRight;
	float costSplit;
	//outside of forloop as it doesn't change.

	AABB aabb, newRightAABB, newLeftAABB;
	int tempLeftFirst = node.first, tempLeftCount, tempRightFirst, tempRightCount;
	int bestLeftCount = node.count / 2;
	//split into a certain number (ex 4/8/16 preferably 8)
	for ( int i = 1; i < 22; i++ ) 
	{
		tempLeftCount = 0;
		aabb.min = node.bounds.min;
		if (i < 8)
			aabb.max = ( vec3( ( node.bounds.min.x + ( ( size.x / 8 ) * i ) ), node.bounds.max.y, node.bounds.max.z ) );
		else if (i < 15)
			aabb.max = vec3( node.bounds.min.x, node.bounds.max.y + ( ( size.y / 8 ) * (i-7)  ), node.bounds.max.z );
		else if ( i < 22 )
			aabb.max = vec3( node.bounds.min.x, node.bounds.max.y + ( ( size.y / 8 ) * (i-14)  ), node.bounds.max.z );

		for (int j = node.first; j < (node.first +node.count); j++)
		{
			if (aabb.Contains(GetPosition(j)))
			{
				tempLeftCount += 1;
			}
		}
		if ( tempLeftCount == 0 )
			continue; 
		float costSplit = SplitCost( node.count, node.first, tempLeftCount );
		if ( costSplit < smallestCost )
		{
			smallestCost = costSplit;
			bestLeftCount = tempLeftCount;
		}
	}

	if ( smallestCost + 0.0001f >= costParent )
	{
		node.isLeaf = true;
		//SplitNode( nodeIdx );
		return;
	}

	//final split
	left.first = node.first;
	left.count = node.count / 2;
	right.first = left.first + left.count;
	right.count = node.count - left.count;
	left.bounds = CalculateBounds( left.first, left.count );
	right.bounds = CalculateBounds( right.first, right.count );
}

bool BVH::IntersectRecursive( Ray &r, RayHit &hit, int nodeIdx )
{
	++r.bvhDepth;
	bool hitAnything = false;
	BVHNode &current = pool[nodeIdx];
	if ( current.isLeaf )
	{
		hitAnything |= IntersectNode( current, r, hit );
	}
	else
	{
		BVHNode &left = pool[current.left];
		if ( left.bounds.Intersect( r ) )
			hitAnything |= IntersectRecursive( r, hit, current.left );
		BVHNode &right = pool[current.left + 1];
		if ( right.bounds.Intersect( r ) )
			hitAnything |= IntersectRecursive( r, hit, current.left + 1 );
	}
	return hitAnything;
}

bool BVH::Intersect( Ray &r, RayHit &hit )
{
	return IntersectRecursive( r, hit, 0);

	// While method, doesnt work better than recursive cause of queue<int> allocation?
	// having static/member open is not thread-safe

	/*queue<int> open = queue<int>();
	open.emplace( 0 );
	bool hitAnything = false;
	while ( open.size() > 0 )
	{
		int currentIdx = open.front();
		BVHNode &current = pool[currentIdx];
		open.pop();
		if ( current.count < maxObjectsPerLeaf )
		{
			hitAnything |= IntersectNode( current, r, hit );
		}
		else
		{
			int leftIdx = currentIdx * 2 + 1;
			if ( pool[leftIdx].bounds.Intersect( r ) )
				open.push( leftIdx );
			int rightIdx = currentIdx * 2 + 1;
			if ( pool[rightIdx].bounds.Intersect( r ) )
				open.push( rightIdx );
		}
	}
	return hitAnything;*/
}

