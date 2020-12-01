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
    bool Trace( Scene *scene, Ray ray, RayHit &hit ); 
	void Fresnel( float sinTheta, float &reflectance, float &cosi, float etat, float etai );
};

