#pragma once

class TriangleBVH : public BVH
{
  public:
	TriangleBVH( MeshObject* mesh ) : mesh( mesh ) {}

	void ConstructBVH();
	AABB CalculateBounds( int first, int count );
	bool IntersectNode( BVHNode &node, Ray &r, RayHit &hit );

	MeshObject *mesh;
};
