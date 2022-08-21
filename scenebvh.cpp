#include "precomp.h"

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
	for ( int i = 0; i < node.objIndices.size(); i++ )
	{
		shared_ptr<HittableObject> obj = scene->objects.at( node.objIndices[i] );
		hitAnything |= obj->Hit( r, hit );
	}
	return hitAnything;
}

