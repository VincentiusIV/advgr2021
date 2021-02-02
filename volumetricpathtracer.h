#pragma once
#include "raytracer.h"

class Ray;
class RayHit;
class Scene;
class Material;

class VolumetricPathTracer : public RayTracer
{
  public:
	VolumetricPathTracer( Scene *scene, int maxDepth ) : RayTracer( scene, maxDepth ) { }
	color Sample( Ray &ray, RayHit &hit );

	color DirectIllumination( point3 &point, vec3 &normal, color &BRDF );
};
