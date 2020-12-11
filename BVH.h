#pragma once

struct BVHNode
{
	AABB bounds;
	int leftFirst;
	int count;
};

struct AABB
{
  public:
	AABB() : min(), max() {}
	AABB(const vec3 &min, const vec3 &max) : min(min), max(max) {}
	bool Contains(const point3 &p);
	bool Intersect( const Ray &r );
	vec3 min, max;
};

class BVH
{
  public:
	BVH();
	
	void ConstructBVH( Scene *scene );
	AABB CalculateBounds( Scene *scene, int first, int count );
	void Subdivide( BVHNode &node );
	void Partition( BVHNode &node );
	bool Intersect( Ray &r, RayHit &hit );
	void FindSplitPlane();

	int poolPtr;
	BVHNode root;
	BVHNode *pool;
};

