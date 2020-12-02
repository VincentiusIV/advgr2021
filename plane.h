#pragma once

#include "object.h"

struct RayHit;

class Plane : public HittableObject
{
	// define hittable plane surface.
 public:
	Plane( shared_ptr<Material> material, vec3 normal );
	Plane( shared_ptr<Material> material, vec3 normal, float width, float height );
	bool Hit( Ray &ray, RayHit &hit );
	vec3 planeNormal;
	float width, height; // TODO: Implement in hit function
};