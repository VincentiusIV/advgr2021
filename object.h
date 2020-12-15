#pragma once

#include "transform.h"
#include "material.h"
#include "ray.h"

struct AABB
{
  public:
	AABB() : min(), max() {}
	AABB( const vec3 &min, const vec3 &max ) : min( min ), max( max ) {}
	bool Contains( const point3 &p );
	bool Intersect( const Ray &r );
	vec3 min, max;
};

class HittableObject : public Transform
{
  public:
	HittableObject( shared_ptr<Material> material ) : Transform(), material( material ) {}
	void Update() { /* implement this if you want to animate it or do other cool stuff*/ }
	virtual void Translate(vec3 delta)
	{
		position += delta;
		UpdateTRS();
		UpdateAABB();
	}
	virtual bool Hit( Ray &ray, RayHit& hit ) { return false; }
	virtual point3 GetRandomPoint() { return position;  }
	virtual vec3 GetNormalAtPoint( const point3 &point ) { return vec3( 0, 0, 0 ); }
	virtual float GetArea() { return 1.0f; }
	shared_ptr<Material> material; 
	AABB aabb;

	virtual void UpdateAABB() { aabb.min = aabb.max = position; }
};