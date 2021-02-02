#pragma once
#include "raytracer.h"

class Ray;
class RayHit;
class Scene;
class Material;


class PathTracer : public RayTracer
{
  public:
	PathTracer( Scene *scene, int maxDepth ) : RayTracer( scene, maxDepth ) {}
	color Sample( Ray &ray, RayHit &hit );
	const color &IndirectIllumination( Ray &ray, RayHit &hit, color &BRDF, RayHit &indirectHit );
	const color &DirectIllumiation( Ray &ray, RayHit &hit, color &BRDF, RayHit &directHit );
};

