#include "precomp.h"

static int MAX_OBJECTS_PER_LEAF = 3;

void BVH::ConstructBVH()
{
	int N = scene->objects.size();
	pool = new BVHNode[N * 2 - 1];
	root = &pool[0];
	poolPtr = 1;
	root->first = 0;
	root->count = scene->objects.size();
	root->bounds = CalculateBounds( root->first, root->count );
	Subdivide(0);
}

AABB BVH::CalculateBounds(int first, int count)
{
	vec3 bmin = vec3( 3.40282e+038 ), bmax = vec3( 1.17549e-038 );
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

void BVH::Subdivide( int nodeIdx )
{
	BVHNode &node = pool[nodeIdx];
	if (node.count < MAX_OBJECTS_PER_LEAF)
	{
		node.isLeaf = true;
		return;
	}
	node.isLeaf = false;
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

bool BVH::IntersectRecursive( Ray &r, RayHit &hit, BVHNode &current, int &depth )
{
	++depth;
	bool hitAnything = false;
	if ( current.isLeaf )
	{
		int last = current.first + current.count;
		for ( int i = current.first; i < last; i++ )
		{
			shared_ptr<HittableObject> obj = scene->objects.at( i );
			hitAnything |= obj->Hit( r, hit );
		}
	}
	else
	{
		BVHNode &left = pool[current.left];
		if ( left.bounds.Intersect( r ) )
			hitAnything |= IntersectRecursive( r, hit, left, depth );
		BVHNode &right = pool[current.right];
		if ( right.bounds.Intersect( r ) )
			hitAnything |= IntersectRecursive( r, hit, right, depth );
	}
	return hitAnything;
}


bool BVH::Intersect( Ray &r, RayHit &hit, int &depth )
{
	depth = 0;
	return IntersectRecursive( r, hit, *root, depth );

	queue<int> open = queue<int>();
	open.emplace( 0 );
	bool hitAnything = false;

	while (open.size() > 0)
	{
		int currentIdx = open.front();
		open.pop();
		BVHNode &current = pool[currentIdx];

		if ( current.isLeaf )
		{
			for ( int i = current.first; i < current.first + current.count; i++ )
			{
				hitAnything |= scene->objects.at( i )->Hit( r, hit );
			}
		}
		else
		{
			if ( pool[current.left].bounds.Intersect( r ) )
				open.push( current.left );
			if ( pool[current.right].bounds.Intersect( r ) )
				open.push( current.right );
		}
	} 
	return hitAnything;
}

bool AABB::Contains( const point3 &p )
{
	return p.x > min.x && p.x < max.x &&
		p.y > min.y && p.y < max.y &&
		p.z > min.z && p.z < max.z;
}

bool AABB::Intersect( const Ray &r )
{
	vec3 invD = vec3( 1.0 / r.direction.x, 1.0 / r.direction.y, 1.0 / r.direction.z );
	vec3 t0s = ( min - r.origin ) * invD;
	vec3 t1s = ( max - r.origin ) * invD;

	vec3 tsmaller = vec3( fmin( t0s.x, t1s.x ), fmin( t0s.y, t1s.y), fmin( t0s.z, t1s.z ) );
	vec3 tbigger = vec3( fmax( t0s.x, t1s.x ), fmax( t0s.y, t1s.y ), fmax( t0s.z, t1s.z ) );

	float tmin = fmax( 0.0f, fmax( tsmaller.x, fmax( tsmaller.y, tsmaller.z ) ) );
	float tmax = fmin( r.tMax, fmin( tbigger.x, fmin( tbigger.y, tbigger.z ) ) );

	return ( tmin < tmax );
}
