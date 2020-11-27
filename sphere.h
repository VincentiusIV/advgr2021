#pragma once

#include "object.h"

struct RayHit;

class Sphere : public HittableObject
{
public:
	Sphere( shared_ptr<Material> material, float radius );
	bool Hit( Ray &ray, RayHit &hit );
	float r;
	float r2; // r*r
};