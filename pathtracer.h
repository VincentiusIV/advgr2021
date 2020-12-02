#pragma once
#include "raytracer.h"

class Ray;
class RayHit;
class Scene;
class Material;


class PathTracer : public RayTracer
{
  public:
	PathTracer( int maxDepth ) : RayTracer( maxDepth ) {}
	color Sample( Ray ray, Scene *scene );
	const color &LitMethod1( Scene *scene, Ray &ray, RayHit &hit, color &BRDF );
	const color &LitMethod2( Scene *scene, Ray &ray, RayHit &hit, color &BRDF );
	bool Trace( Scene *scene, Ray ray, RayHit &hit );
	bool Trace( Scene *scene, Ray ray, RayHit &hit, MaterialType typeToIgnore ); 
	void Fresnel( float sinTheta, float &reflectance, float &cosi, float etat, float etai );
};

