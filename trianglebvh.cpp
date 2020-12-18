#include "precomp.h"

AABB TriangleBVH::CalculateBounds( int first, int count )
{
	int j = first * 3;
	vec3 bmin = vec3( 3.40282e+038 ), bmax = vec3( 1.17549e-038 );
	for ( int i = 0; i < count; i++ )
	{
		int i0 = mesh->indices[j];
		int i1 = mesh->indices[j + 1];
		int i2 = mesh->indices[j + 2];
		vec3 v0 = mesh->worldVertices[i0];
		vec3 v1 = mesh->worldVertices[i1];
		vec3 v2 = mesh->worldVertices[i2];
		bmin = MinPerAxis( bmin, MinPerAxis( MinPerAxis( v0, v1 ), v2) );
		bmax = MaxPerAxis( bmax, MaxPerAxis( MaxPerAxis( v0, v1 ), v2 ) );
		j += 3;
	}
	if (bmax.x - bmin.x == 0.0f)
	{
		bmax.x += 0.001f;
		bmin.x -= 0.001f;
	}
	if ( bmax.y - bmin.y == 0.0f )
	{
		bmax.y += 0.001f;
		bmin.y -= 0.001f;
	}
	if ( bmax.z - bmin.z == 0.0f )
	{
		bmax.z += 0.001f;
		bmin.z -= 0.001f;
	}

	return AABB( bmin, bmax );
}

bool TriangleBVH::IntersectNode( BVHNode &node, Ray &r, RayHit &hit )
{	
	bool hitAnything = false;
	uint j = node.first*3;
	for ( int i = 0; i < node.count; i++ )
	{
		vec3 v0 = mesh->worldVertices[mesh->indices[j]];
		vec3 v1 = mesh->worldVertices[mesh->indices[j + 1]];
		vec3 v2 = mesh->worldVertices[mesh->indices[j + 2]];
		if (MeshObject::CheckRayTriangleIntersection(r, hit, v0, v1, v2))
		{
			hitAnything = true;
			//hit.normal = mesh->normals[mesh->indices[j]];
		}
		j += 3;
	}
	return hitAnything;
}
