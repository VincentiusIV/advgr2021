#pragma once

#include "transform.h"

class Light : public Transform
{
  public:
	Light( Point3 position, float intensity ) : Transform(position), intensity( intensity ) {}
	
	float intensity;

};