#pragma once

#include "object.h"

struct RayHit;

class Sphere : public HittableObject
{
public:
	Sphere( shared_ptr<Material> material, float radius );
	bool Hit( Ray &ray, RayHit &hit );
	void UpdateAABB();
	point3 GetRandomPoint();
	vec3 GetNormalAtPoint( const point3 &point );
	float GetArea() { return powf( 4.0f * PI * r, 2.0f ); }
	bool Contains( point3 point ) { return ( point - position).length() <= r; }
	float r;
	float r2; 
};