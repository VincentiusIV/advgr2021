#pragma once

#include "object.h"

struct RayHit;

class Sphere : public HittableObject
{
public:
	Sphere( shared_ptr<Material> material, float radius );
	bool Hit( Ray &ray, RayHit &hit );
	static bool Hit( Ray &ray, RayHit &hit, point3 spherePos, float radius );
	static bool Hit( Ray &ray, RayHit &hit, point3 spherePos, float radius, float &t );
	void UpdateAABB();
	point3 GetRandomPoint();
	vec3 GetNormalAtPoint( const point3 &point );
	float GetArea() { return powf(4.0f * PI * r, 2.0f); }
	float r;
	float r2; // r*r
};