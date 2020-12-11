#include "precomp.h"

BVH::BVH()
{
}

void BVH::ConstructBVH( Scene *scene )
{
	int N = scene->objects.size();
	pool = new BVHNode[N * 2 - 1];
	root = pool[0];
	poolPtr = 2;

	root.leftFirst = 0;
	root.count = scene->objects.size();
	root.bounds = CalculateBounds( scene, root.leftFirst, root.count );
	Subdivide(root);
}

aabb BVH::CalculateBounds( Scene *scene, int first, int count )
{
	return aabb();
}

void BVH::Subdivide( BVHNode &node )
{
	if (node.count < 3)
		return;
	node.leftFirst = poolPtr;
	poolPtr += 2;
	Partition(node);
	Subdivide( pool[node.leftFirst] );
	Subdivide( pool[node.leftFirst+1] );
}

void BVH::Partition( BVHNode &node )
{
}

bool BVH::Intersect( Ray &r, RayHit &hit )
{
	return false;
}

void BVH::FindSplitPlane()
{

}
