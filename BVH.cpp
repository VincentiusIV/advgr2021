#include "precomp.h"

void BVH::ConstructBVH()
{
	printf( "Started BVH Construction\n" );
	timer t;
	t.reset();
	pool = new BVHNode[N * 2 - 1];
	root = &pool[0];
	root->first = 0;
	root->count = N;
	root->bounds = CalculateBounds( root->first, root->count );
	Subdivide( 0 );
	string timeString = "Finished BVH Construction after t=" + to_string( t.elapsed() ) + "\n";
	printf( timeString.c_str());
}

void BVH::Subdivide( int nodeIdx )
{
	BVHNode &node = pool[nodeIdx];
	if (node.count < maxObjectsPerLeaf)
	{
		return;
	}
	SplitNode(nodeIdx);
	Subdivide( nodeIdx * 2 + 1);
	Subdivide( nodeIdx * 2 + 2);
}

void BVH::SplitNode( int nodeIdx )
{
	BVHNode &node = pool[nodeIdx];
	BVHNode &left = pool[nodeIdx*2+1];
	left.first = node.first;
	left.count = node.count / 2;
	left.bounds = CalculateBounds( left.first, left.count );

	BVHNode &right = pool[nodeIdx * 2 + 2];
	right.first = left.first+left.count;
	right.count = node.count - left.count;
	right.bounds = CalculateBounds( right.first, right.count );
}

void BVH::SplitNodeSAH(int nodeIdx)
{
	BVHNode &node = pool[nodeIdx];
	BVHNode &left = pool[nodeIdx * 2 + 1];
	BVHNode &right = pool[nodeIdx * 2 + 2];
	
	//node.bounds = CalculateBounds( node.first, node.count );
	float areaNode = ( node.bounds.max.x - node.bounds.min.x ) * ( node.bounds.max.y - node.bounds.min.y ) * ( node.bounds.max.z - node.bounds.min.z );
	float costParent = areaNode * node.count;
	float smallestCost = costParent;
	float perfSplit = node.count/2;

	float areaLeft;
	float areaRight;
	float costSplit;

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
		areaLeft = ( left.bounds.max.x - left.bounds.min.x ) * ( left.bounds.max.y - left.bounds.min.y ) * ( left.bounds.max.z - left.bounds.min.z );

		right.bounds = CalculateBounds(right.first, right.count);
		areaRight = ( right.bounds.max.x - right.bounds.min.x ) * ( right.bounds.max.y - right.bounds.min.y ) * ( right.bounds.max.z - right.bounds.min.z );
		
		costSplit = ( areaLeft * left.count ) + ( areaRight * right.count );

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
	//split into a certain number (ex 4/8/16 preferably 8)

	BVHNode &node = pool[nodeIdx];
	BVHNode &left = pool[node.left];
	BVHNode &right = pool[node.right];

	//node.bounds = CalculateBounds( node.first, node.count );
	float widthP = node.bounds.max.x - node.bounds.min.x;
	float areaNode = ( widthP ) * ( node.bounds.max.y - node.bounds.min.y ) * ( node.bounds.max.z - node.bounds.min.z );
	float costParent = areaNode * node.count;

	

	float smallestCost = costParent;
	float perfSplit = node.count / 2;

	float areaLeft;
	float areaRight;
	float costSplit;

	//this stays the same during the whole for loop, so it can stay outside of the forloop
	left.first = node.first;

	//Split the plane on each primitive. When splitting on a primitive, it goes to the right side.
	for ( int i = 1; i < 8; i++ ) //we start at i=1, as we do not care about the first split as it would give an empty left node.
	{
		AABB aabb = AABB(node.bounds.min.x, ((widthP / 8)*i));
		for (int i = 1; i < node.count; i++)
		{
			//shared_ptr<HittableObject> obj = scene->objects.at( i );

			if (aabb.Contains(i)) //niet pointer i, maar i'ste object en dan de locatie daarvan
			{
				left.count += 1;
			}
		}

		//left.count = i * (ceilf(node.count/8)); //obj.i is not in the left node, however the list starts at 0 but you do not have 0 objects

		right.first = left.first + left.count;
		right.count = node.count - left.count;

		left.bounds = CalculateBounds( left.first, left.count );
		areaLeft = ( left.bounds.max.x - left.bounds.min.x ) * ( left.bounds.max.y - left.bounds.min.y ) * ( left.bounds.max.z - left.bounds.min.z );

		right.bounds = CalculateBounds( right.first, right.count );
		areaRight = ( right.bounds.max.x - right.bounds.min.x ) * ( right.bounds.max.y - right.bounds.min.y ) * ( right.bounds.max.z - right.bounds.min.z );

		costSplit = ( areaLeft * left.count ) + ( areaRight * right.count );

		//check if the split with obj i is smaller than the previous split
		if ( costSplit < smallestCost )
		{
			smallestCost = costSplit;
			perfSplit = i;
		}
		//Don't do 'not greedy' way!(child cost split: Aleft0* (Aleft1*Nleft1 + Aright1*Nright1 ) + Aright0 *(etc)
		//greedy == assume that the split is for a leaf node (last split)
	}

	// split is 'not worth it' if cost is higher than cost of parent node
	//^this decides when you stop as well
	if ( smallestCost + 0.000001 >= costParent )
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
		int leftIdx = nodeIdx * 2 + 1;
		BVHNode &left = pool[leftIdx];
		if ( left.bounds.Intersect( r ) )
			hitAnything |= IntersectRecursive( r, hit, leftIdx );
		int rightIdx = nodeIdx * 2 + 2;
		BVHNode &right = pool[rightIdx];
		if ( right.bounds.Intersect( r ) )
			hitAnything |= IntersectRecursive( r, hit, rightIdx );
	}
	return hitAnything;
}


bool BVH::Intersect( Ray &r, RayHit &hit )
{
	return IntersectRecursive( r, hit, 0);

	// While method, doesnt work better than recursive cause of queue<int> allocation?

	queue<int> open = queue<int>();
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
	return hitAnything;
}

