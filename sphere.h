#pragma once

#include "object.h"

struct RayHit;

class Sphere : public HittableObject
{
public:
	Sphere(shared_ptr<Material> material, float radius) : HittableObject(material), radius(radius) {}

	bool Hit( Ray &ray, RayHit &hit ) 
	{
		vec3 C = position - ray.origin;
		float t = dot( C, ray.direction );
		vec3 Q = C - t * ray.direction;
		float p2 = dot( Q, Q );
		if ( p2 > radius )
			return false;
		hit.material = material;
		hit.point = ray.At( t );
		
		return true;
	}

	float radius;
};