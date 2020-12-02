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
	color Sample( Ray &ray );
	const color &LitMethod1( Ray &ray, RayHit &hit, color &BRDF );
	const color &LitMethod2( Ray &ray, RayHit &hit, color &BRDF );

};

