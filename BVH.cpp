#include "precomp.h"
#include <omp.h>

void BVH::ConstructBVH()
{
	std::string notifyString = "Started BVH Construction for N=" + to_string( N ) + " \n"; 
	printf( notifyString.c_str());
	timer t;
	t.reset();
	nodeCount = N * 2 - 1;
	pool = new BVHNode[nodeCount];
	root = &pool[0];
	poolPtr = 2;
	root->first = 0;
	root->count = N;
	root->bounds = CalculateBounds( root->first, root->count );

//	Subdivide( 0, nodeCount );
	progressCounter = 0;
	if (false)
	{
		int segmentCount = 4;
		int segment = fmax( 1, ( N * 2 - 1 ) / segmentCount );

		#pragma omp parallel for schedule( dynamic, 1 )
		for ( int i = 0; i < segmentCount; i++ )
		{
			int nodeIdx = ( i * segment );
			int lastIdx = fmin( nodeCount, ( nodeIdx + segment ) );
			Subdivide( nodeIdx, lastIdx );
		}
	}
	else
	{
		Subdivide( 0, nodeCount);	
	}

	string timeString = "Finished BVH Construction after t=" + to_string( t.elapsed() ) + "\n";
	printf( timeString.c_str());
}

void BVH::Subdivide( int nodeIdx, int maxNodeIdx )
{
	++progressCounter;
	std::cerr << "\rBVH subdivisions: " << double(progressCounter) << ' ' << std::flush;
	BVHNode &node = pool[nodeIdx];
	if (node.count < maxObjectsPerLeaf || nodeIdx >= maxNodeIdx)
	{
		return;
	}
	node.left = poolPtr++;
	node.right = poolPtr++;
	SplitNodeSAH(nodeIdx);
	Subdivide( node.left, maxNodeIdx );
	Subdivide( node.right, maxNodeIdx );
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

void BVH::SplitNodeSAH(int nodeIdx)
{
	if ( nodeIdx >= nodeCount )
		return;

	BVHNode &node = pool[nodeIdx];
	BVHNode &left = pool[node.left];
	BVHNode &right = pool[node.right];

	//node.bounds = CalculateBounds( node.first, node.count );
	float areaNode = ( node.bounds.max.x - node.bounds.min.x ) * ( node.bounds.max.y - node.bounds.min.y ) * ( node.bounds.max.z - node.bounds.min.z );
	float costParent = areaNode * node.count;
	float smallestCost = costParent;
	int perfSplit = node.count/2;


	//this stays the same during the whole for loop, so it can stay outside of the forloop
	left.first = node.first; 

	//Split the plane on each primitive. When splitting on a primitive, it goes to the right side. 
	for (int i = 1; i< node.count; i++) //we start at i=1, as we do not care about the first split as it would give an empty left node.
	{
		//object i; 
		//shared_ptr<HittableObject> obj = scene->objects.at( i );
		//obj.pos;

		left.count = i; //obj.i is not in the left node, however the list starts at 0 but you do not have 0 objects

		right.first = left.first + left.count;
		right.count = node.count - left.count; 

		left.bounds = CalculateBounds(left.first, left.count);
		float areaLeft = ( left.bounds.max.x - left.bounds.min.x ) * ( left.bounds.max.y - left.bounds.min.y ) * ( left.bounds.max.z - left.bounds.min.z );

		right.bounds = CalculateBounds(right.first, right.count);
		float areaRight = ( right.bounds.max.x - right.bounds.min.x ) * ( right.bounds.max.y - right.bounds.min.y ) * ( right.bounds.max.z - right.bounds.min.z );
		
		float costSplit = ( areaLeft * left.count ) + ( areaRight * right.count );

		//check if the split with obj i is smaller than the previous split
		if (costSplit < smallestCost)
		{
			smallestCost = costSplit;
			perfSplit = i;
		}
		//Don't do 'not greedy' way!(child cost split: Aleft0* (Aleft1*Nleft1 + Aright1*Nright1 ) + Aright0 *(etc)
		//greedy == assume that the split is for a leaf node (last split)
	}
		
	// split is 'not worth it' if cost is higher than cost of parent node 
	//^this decides when you stop as well
	if(smallestCost + 0.000001 >= costParent) 
	{
		//return leaf; //no split
		//TODO: move to subdivide
	}

	//final split
	left.count = perfSplit;

	right.first = left.first + left.count;
	right.count = node.count - left.count; 

	left.bounds = CalculateBounds( left.first, left.count );
	right.bounds = CalculateBounds( right.first, right.count );
}

void BVH::SplitNodeBin(int nodeIdx)
{

}

bool BVH::IntersectRecursive( Ray &r, RayHit &hit, int nodeIdx )
{
	++r.bvhDepth;
	bool hitAnything = false;
	BVHNode &current = pool[nodeIdx];
	if ( current.count < maxObjectsPerLeaf )
	{
		hitAnything |= IntersectNode( current, r, hit );
	}
	else
	{
		BVHNode &left = pool[current.left];
		if ( left.bounds.Intersect( r ) )
			hitAnything |= IntersectRecursive( r, hit, current.left );
		BVHNode &right = pool[current.right];
		if ( right.bounds.Intersect( r ) )
			hitAnything |= IntersectRecursive( r, hit, current.right );
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

