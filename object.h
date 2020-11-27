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
	
	shared_ptr<Material> material;
};