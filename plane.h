#pragma once

#include "object.h"

struct RayHit;

class Plane : public HittableObject
{
 public:
	Plane( shared_ptr<Material> material, vec3 normal );
	Plane( shared_ptr<Material> material, vec3 normal, float width, float height );
	bool Hit( Ray &ray, RayHit &hit );
	void UpdateAABB();
	point3 GetRandomPoint();
	vec3 GetNormalAtPoint( const point3 &point ) { return planeNormal; }
	float GetArea() { return ( scale.x * scale.y * scale.z); }
	vec3 planeNormal;
	float width, height; 
};