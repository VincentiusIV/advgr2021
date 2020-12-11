#include "precomp.h"

static int MAX_OBJECTS_PER_LEAF = 3;

void BVH::ConstructBVH()
{
	int N = scene->objects.size();
	pool = new BVHNode[N * 2 - 1];
	root = pool[0];
	poolPtr = 2;
	root.leftFirst = 0;
	root.count = scene->objects.size();
	root.bounds = CalculateBounds( root.leftFirst, root.count );
	Subdivide(root);
}

AABB BVH::CalculateBounds(int first, int count )
{
	vec3 bmin = vec3(), bmax = vec3();
	for ( size_t i = first; i < first+count; i++ )
	{
		shared_ptr<HittableObject> obj = scene->objects.at( i );
		if ( obj->aabb.min.x < bmin.x )
			bmin.x = obj->aabb.min.x;
		if ( obj->aabb.min.y < bmin.y )
			bmin.y = obj->aabb.min.y;
		if ( obj->aabb.min.z < bmin.z )
			bmin.z = obj->aabb.min.z;

		if ( obj->aabb.max.x > bmax.x )
			bmax.x = obj->aabb.max.x;
		if ( obj->aabb.max.y > bmax.y )
			bmax.y = obj->aabb.max.y;
		if ( obj->aabb.max.z > bmax.z )
			bmax.z = obj->aabb.max.z;
	}
	return AABB( bmin, bmax );
}

void BVH::Subdivide( BVHNode &node )
{
	if ( node.count < MAX_OBJECTS_PER_LEAF  )
		return;
	node.leftFirst = poolPtr;
	poolPtr += 2;
	SplitNode(node);
	Subdivide( pool[node.leftFirst] );
	Subdivide( pool[node.right()] );
}

bool BVH::Intersect( Ray &r, RayHit &hit )
{
	vector<int> open = vector<int>();
	open.push_back( 0 );
	bool hitAnything = false;
	while (open.size() > 0)
	{
		BVHNode current = pool[open.at(0)];
		open.erase( open.begin() );

		if (current.count >= MAX_OBJECTS_PER_LEAF)
		{
			BVHNode left = pool[current.leftFirst];
			BVHNode right = pool[current.right()];
			if ( left.bounds.Intersect( r ) )
				open.push_back( current.leftFirst );
			if ( right.bounds.Intersect( r ) )
				open.push_back( current.right() );
		}
		else
		{
			for ( size_t i = current.leftFirst; i < current.leftFirst + current.count; i++ )
			{
				shared_ptr<HittableObject> obj = scene->objects.at( i );
				hitAnything |= obj->Hit( r, hit );
			}
		}
	} 
	return hitAnything;
}

void BVH::SplitNode( BVHNode &node )
{
	int leftFirst = node.leftFirst;
	int leftCount = node.count / 2;
	int rightFirst = node.right();
	int rightCount = node.count - leftCount;

	AABB left = CalculateBounds( leftFirst, leftCount );
	AABB right = CalculateBounds( rightFirst, rightCount);

	pool[node.leftFirst].leftFirst = leftFirst;
	pool[node.leftFirst].count = leftCount;
	pool[node.leftFirst].bounds = left;

	pool[node.right()].leftFirst = rightFirst;
	pool[node.right()].count = rightCount;
	pool[node.right()].bounds = right;

	//vec3 sizeDiff = node.bounds.max - node.bounds.min;
	//AABB left, right;
	//if (sizeDiff.x > sizeDiff.y && sizeDiff.x > sizeDiff.z)
	//{
	//	// Split on x
	//	left = AABB( node.bounds.min, vec3( node.bounds.min.x + sizeDiff.x / 2, node.bounds.max.y, node.bounds.max.z ) );
	//	right = AABB( node.bounds.min + vec3( sizeDiff.x / 2, 0, 0), node.bounds.max );
	//}
	//else if ( sizeDiff.y > sizeDiff.x && sizeDiff.y > sizeDiff.z )
	//{
	//	// Split on y
	//	left = AABB( node.bounds.min, vec3( node.bounds.min.x, node.bounds.max.y + sizeDiff.y / 2, node.bounds.max.z ) );
	//	right = AABB( node.bounds.min + vec3( 0, sizeDiff.y / 2, 0 ), node.bounds.max );
	//}
	//else
	//{
	//	// Split on z
	//	left = AABB( node.bounds.min, vec3( node.bounds.min.x, node.bounds.max.y, node.bounds.max.z + sizeDiff.z / 2 ) );
	//	right = AABB( node.bounds.min + vec3( 0, 0, sizeDiff.z / 2 ), node.bounds.max );
	//}

}

bool AABB::Contains( const point3 &p )
{
	return p.x > min.x && p.x < max.x &&
		p.y > min.y && p.y < max.y &&
		p.z > min.z && p.z < max.z;
}

bool AABB::Intersect( const Ray &r )
{
	float tmin = ( min.x - r.origin.x ) / r.direction.x;
	float tmax = ( max.x - r.origin.x ) / r.direction.x;

	if ( tmin > tmax ) 
		swap( tmin, tmax );

	float tymin = ( min.y - r.origin.y ) / r.direction.y;
	float tymax = ( max.y - r.origin.y ) / r.direction.y;

	if ( tymin > tymax ) 
		swap( tymin, tymax );

	if ( ( tmin > tymax ) || ( tymin > tmax ) )
		return false;

	if ( tymin > tmin )
		tmin = tymin;

	if ( tymax < tmax )
		tmax = tymax;

	float tzmin = ( min.z - r.origin.z ) / r.direction.z;
	float tzmax = ( max.z - r.origin.z ) / r.direction.z;

	if ( tzmin > tzmax ) 
		swap( tzmin, tzmax );

	if ( ( tmin > tzmax ) || ( tzmin > tmax ) )
		return false;

	if ( tzmin > tmin )
		tmin = tzmin;

	if ( tzmax < tmax )
		tmax = tzmax;

	return true;
}

