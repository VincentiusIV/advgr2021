#pragma once

class Ray;
class Scene;

class RayTracer
{
public:
	virtual Color Trace( Ray ray, Scene *scene ) = 0;
};

class WhittedRayTracer : public RayTracer
{
  public:
	WhittedRayTracer() {}

	Color Trace( Ray ray, Scene *scene );
};