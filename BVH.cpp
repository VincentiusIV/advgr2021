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

/*
/*class void BVH()
{
	Boundbox();
	BVHnode* left=0, *right=0 //pointer to childs
	vector<Primitive*> prims;
	bool isLeaf = true;
	void Intersect(Ray &r);
	void Subdivide();
	FindSplitPlane();
}

void BoundBox(MeshObject o) //one for mesh, one for circles, one for planes
{
	vec3 bmin, bmax; 
	for()
	{
		if (p.x[i] < bmin.x) bmin.x = p.x[i];
		if (p.y[i] < bmin.y) bmin.y = p.y[i];
		if (p.z[i] < bmin.z) bmin.z = p.z[i];
		if (p.x[i] > bmax.x) bmax.x = p.x[i];
		if (p.y[i] > bmax.y) bmax.y = p.y[i];
		if (p.z[i] > bmax.z) bmax.z = p.z[i];
	}

	return bmin bmax; 
}


void FindSplitPlane()
{
//pos and axis
}

void Intersect(Ray &r)
{
}

void Subdivide()
{
}*/
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
