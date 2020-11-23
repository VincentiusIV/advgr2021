#pragma once

#include "object.h"

struct RayHit;

class Plane : public HittableObject
{
	// define hittable plane surface.
 public:
	Plane( shared_ptr<Material> material, vec3 normal );

	bool Hit( Ray &ray, RayHit &hit );

	vec3 normalP; //normal vector of plane
	// Plane: P . N(vec) + d = 0
};