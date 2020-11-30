#pragma once
#include "raytracer.h"

class Ray;
class RayHit;
class Scene;
class Material;


class PathTracer : public RayTracer
{
  public:
	Color Sample( Ray ray, Scene *scene );

    bool Trace( Scene *scene, Ray ray, RayHit &hit );
 
};

