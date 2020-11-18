#pragma once

#include "transform.h"
#include "material.h"
#include "ray.h"

class HittableObject : public Transform
{
public:
	HittableObject(shared_ptr<Material> material) : Transform(), material(material) {}
	~HittableObject() {}
	void Update();
	virtual bool Hit( Ray &ray, RayHit& hit ) { return false; }
	
	shared_ptr<Material> material;
};