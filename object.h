#pragma once

#include "transform.h"
#include "material.h"
#include "ray.h"

class HittableObject : public Transform
{
  public:
	HittableObject( shared_ptr<Material> material ) : Transform(), material( material ) {}
	void Update() { /* implement this if you want to animate it or do other cool stuff*/ }
	virtual bool Hit( Ray &ray, RayHit& hit ) { return false; }
	virtual point3 GetRandomPoint() { return position;  }
	virtual vec3 GetNormalAtPoint( const point3 &point ) { return vec3( 0, 0, 0 ); }
	virtual float GetArea() { return 1.0f; }
	shared_ptr<Material> material; 
};