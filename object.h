#pragma once

#include "transform.h"
#include "material.h"

class HittableObject : public Transform
{
public:
	HittableObject();
	~HittableObject();
	void Update();

	
};