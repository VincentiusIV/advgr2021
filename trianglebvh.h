#pragma once

class TriangleBVH : public BVH
{
  public:
	TriangleBVH( MeshObject* mesh ) : mesh( mesh ) {}
	void ConstructBVH();
	AABB CalculateBounds( int first, int count );
	bool IntersectNode( BVHNode &node, Ray &r, RayHit &hit );
	vec3 GetPosition( int objIdx ) { return mesh->worldVertices[mesh->indices[objIdx * 3]]; }
	MeshObject *mesh;
};
