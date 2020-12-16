#include "precomp.h"

void SceneBVH::ConstructBVH()
{
	N = scene->objects.size();
	BVH::ConstructBVH();
}

AABB SceneBVH::CalculateBounds( int first, int count )
{
	vec3 bmin = vec3( 3.40282e+038 ), bmax = vec3( 1.17549e-038 );
	for ( size_t i = first; i < first + count; i++ )
	{
		shared_ptr<HittableObject> obj = scene->objects.at( i );
		bmin = MinPerAxis( bmin, obj->aabb.min );
		bmax = MaxPerAxis( bmax, obj->aabb.max );
	}
	return AABB( bmin, bmax );
}

bool SceneBVH::IntersectNode( BVHNode &node, Ray &r, RayHit &hit )
{
	bool hitAnything = false;
	int last = node.first + node.count;
	for ( int i = node.first; i < last; i++ )
	{
		shared_ptr<HittableObject> obj = scene->objects.at( i );
		hitAnything |= obj->Hit( r, hit );
	}
	return hitAnything;
}

