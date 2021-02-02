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

	root->bounds = CalculateBounds( 0, N );

	root->objIndices = vector<uint>();
	for ( int i = 0; i < N; i++ )
	{
		root->objIndices.push_back( i );
	}

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
	//std::cerr << "\rBVH subdivisions: " << double(progressCounter) << ' ' << std::flush;
	if ( node.isLeaf || node.objIndices.size() < maxObjectsPerLeaf )
	{
		node.isLeaf = true;
		return;
	}
	node.left = poolPtr;
	poolPtr += 2;
	SplitNodeBin( nodeIdx );
	Subdivide( node.left, maxNodeIdx );
	Subdivide( node.left+1, maxNodeIdx );

	//EnsureCorrectSize();
}

void BVH::EnsureCorrectSize()
{
	for ( int i = 0; i < nodeCount; i++ )
	{
		BVHNode &node = pool[i];
		for ( int i = 0; i < node.count(); i++ )
		{
			AABB aabb = GetObjectAABB( node.objIndices[i] );
			node.bounds.min = MinPerAxis( node.bounds.min, aabb.min );
			node.bounds.max = MaxPerAxis( node.bounds.max, aabb.max );
		}
	}
}

void BVH::SplitNodeSAH(int nodeIdx)
{
	BVHNode &node = GetNode( nodeIdx );
	BVHNode &left = GetLeftNode( nodeIdx );
	BVHNode &right = GetRightNode( nodeIdx );

	vec3 size = node.bounds.max - node.bounds.min;
	float costParent = node.bounds.Area() * node.count();
	float smallestCost = costParent;

	AABB leftBin, rightBin, bestLeftBin, bestRightBin;
	vector<uint> leftIndices = vector<uint>(), rightIndices = vector<uint>(), bestLeftIndices = vector<uint>(), bestRightIndices = vector<uint>();

	for ( int i = 1; i < node.count()*3; i++ )
	{
		leftIndices.clear();
		rightIndices.clear();

		// Make new bins
		leftBin.min = node.bounds.min;
		rightBin.max = node.bounds.max;

		if ( i < node.count() ) // test x-axis split
		{
			uint objIdx = node.objIndices[i];
			vec3 objPos = GetObjectPosition( objIdx ); 
			leftBin.max = ( vec3( objPos.x, node.bounds.max.y, node.bounds.max.z ) );
			rightBin.min = node.bounds.min;
			rightBin.min.x = leftBin.max.x;
		}
		else if ( i < node.count() * 2 ) // text y-axis split
		{
			uint objIdx = node.objIndices[i - node.count()];
			vec3 objPos = GetObjectPosition( objIdx ); 
			leftBin.max = vec3( node.bounds.max.x, objPos.y, node.bounds.max.z );
			rightBin.min = node.bounds.min;
			rightBin.min.y = leftBin.max.y;
		}
		else // text z-axis split
		{
			uint objIdx = node.objIndices[i - node.count()*2];
			vec3 objPos = GetObjectPosition( objIdx ); 
			leftBin.max = vec3( node.bounds.max.x, node.bounds.max.y, objPos.z );
			rightBin.min = node.bounds.min;
			rightBin.min.z = leftBin.max.z;
		}

		// Test which objects are in which bin
		for ( int j = 0; j < node.objIndices.size(); j++ )
		{
			int x = node.objIndices[j];
			if ( leftBin.Contains( GetObjectPosition( x ) ) )
			{
				leftIndices.push_back( x );
			}
			else
			{
				rightIndices.push_back( x );
			}
		}

		if ( leftIndices.size() == 0 )
			continue;

		float costSplit = ( leftBin.Area() * leftIndices.size() ) + ( rightBin.Area() * rightIndices.size() );
		if ( costSplit < smallestCost )
		{
			smallestCost = costSplit;
			bestLeftBin = leftBin;
			bestRightBin = rightBin;
			bestLeftIndices.clear();
			for ( int i = 0; i < leftIndices.size(); i++ )
				bestLeftIndices.push_back( leftIndices[i] );
			bestRightIndices.clear();
			for ( int i = 0; i < rightIndices.size(); i++ )
				bestRightIndices.push_back( rightIndices[i] );
		}
	}

	if ( smallestCost >= costParent )
	{
		node.isLeaf = true;
		return;
	}

	left.bounds = bestLeftBin;
	for (int i = 0; i < bestLeftIndices.size(); i++)
	{
		left.objIndices.push_back( bestLeftIndices[i] );
		AABB aabb = GetObjectAABB( bestLeftIndices[i] );
		left.bounds.min = MinPerAxis(left.bounds.min, aabb.min );
		left.bounds.max = MaxPerAxis(left.bounds.max, aabb.max );
	}
	right.bounds = bestRightBin;
	for (int i = 0; i < bestRightIndices.size(); i++)
	{
		right.objIndices.push_back( bestRightIndices[i] );
		AABB aabb = GetObjectAABB( bestRightIndices[i] );
		right.bounds.min = MinPerAxis( right.bounds.min, aabb.min );
		right.bounds.max = MaxPerAxis( right.bounds.max, aabb.max );
	}
}

void BVH::SplitNodeBin(int nodeIdx)
{
	BVHNode &node = GetNode( nodeIdx );
	BVHNode &left = GetLeftNode( nodeIdx );
	BVHNode &right = GetRightNode( nodeIdx );

	vec3 size = node.bounds.max - node.bounds.min;
	float costParent = node.bounds.Area() * node.count();
	float smallestCost = costParent;

	AABB leftBin, rightBin, bestLeftBin, bestRightBin;
	vector<uint> leftIndices = vector<uint>(), rightIndices = vector<uint>(), bestLeftIndices = vector<uint>(), bestRightIndices = vector<uint>();

	for ( int i = 1; i < 22; i++ ) 
	{
		leftIndices.clear();
		rightIndices.clear();

		// Make new bins
		leftBin.min = node.bounds.min;

		if (i < 8) 
		{
			leftBin.max = ( vec3( ( node.bounds.min.x + ( ( size.x / 8 ) * i ) ), node.bounds.max.y, node.bounds.max.z ) );
			rightBin.min = node.bounds.min;
			rightBin.min.x = leftBin.max.x;
		}
		else if (i < 15)
		{
			leftBin.max = vec3( node.bounds.max.x, node.bounds.min.y + ( ( size.y / 8 ) * ( i - 7 ) ), node.bounds.max.z );
			rightBin.min = node.bounds.min;
			rightBin.min.y = leftBin.max.y;
		
		}
		else if ( i < 22 )
		{
			leftBin.max = vec3( node.bounds.max.x, node.bounds.max.y, node.bounds.min.z + ( ( size.z / 8 ) * ( i - 14 ) ) );
			rightBin.min = node.bounds.min;
			rightBin.min.z = leftBin.max.z;
		}
		
		rightBin.max = node.bounds.max;

		for (int j = 0; j < node.objIndices.size(); j++)
		{
			int x = node.objIndices[j];
			AABB aabb = GetObjectAABB( x );
			if ( leftBin.Contains( GetObjectPosition(x ) ) )
			{
				leftIndices.push_back( x );
				leftBin.min = MinPerAxis( leftBin.min, aabb.min );
				leftBin.max = MaxPerAxis( leftBin.max, aabb.max );
			}
			else
			{
				rightIndices.push_back( x );
				rightBin.min = MinPerAxis( rightBin.min, aabb.min );
				rightBin.max = MaxPerAxis( rightBin.max, aabb.max );
			}
		}

		if ( leftIndices.size() == 0 )
			continue; 

		float costSplit = ( leftBin.Area() * leftIndices.size() ) + ( rightBin.Area() * rightIndices.size() );
		if ( costSplit < smallestCost )
		{
			smallestCost = costSplit;
			bestLeftBin = leftBin;
			bestRightBin = rightBin;
			bestLeftIndices.clear();
			for ( int i = 0; i < leftIndices.size(); i++ )
				bestLeftIndices.push_back( leftIndices[i] ); 
			bestRightIndices.clear();
			for ( int i = 0; i < rightIndices.size(); i++ )
				bestRightIndices.push_back( rightIndices[i] ); 
		}
	}

	if ( smallestCost >= costParent )
	{
		node.isLeaf = true;
		return;
	}

	left.bounds = bestLeftBin;
	for ( int i = 0; i < bestLeftIndices.size(); i++ )
	{
		left.objIndices.push_back( bestLeftIndices[i] );
	}

	right.bounds = bestRightBin;
	for ( int i = 0; i < bestRightIndices.size(); i++ )
	{
		right.objIndices.push_back( bestRightIndices[i] );

	}
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

