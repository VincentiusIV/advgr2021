#pragma once

class TriangleBVH : public BVH
{
  public:
	TriangleBVH( MeshObject *mesh, int N ) : BVH( N ) , mesh( mesh )
	{

	}
	AABB CalculateBounds( int first, int count );
	bool IntersectNode( BVHNode &node, Ray &r, RayHit &hit );
	vec3 GetPosition( int objIdx ) 
	{ 
		int i0 = objIdx * 3;
		int index = mesh->indices[i0];
		return mesh->worldVertices[index];
	}
	MeshObject *mesh;
};
