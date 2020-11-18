#pragma once

#include "material.h"

struct RayHit
{
  public:
	Point3 point;
	Vector3 normal;
	shared_ptr<Material> material;
};
