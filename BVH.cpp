#include "precomp.h"

BVH::BVH()
{
}

void BVH::ConstructBVH( Scene *scene )
{

}

bool BVH::Intersect( Ray &r, RayHit &hit )
{

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