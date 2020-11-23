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
	WhittedRayTracer(int maxDepth) : maxDepth(maxDepth) {}

	Color Trace( Ray ray, Scene *scene );
	void Fresnel( float sinTheta, float &reflectance, float &cosi, float etat, float etai );
	int maxDepth;
};