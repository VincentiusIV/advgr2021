#pragma once
#include "raytracer.h"

class Ray;
class RayHit;
class Scene;
class Material;

struct photon
{
  public:
	point3 position; // world space position of the photon hit
	point3 power;	 //current power level for the photon (can also be stored in float 3)
	vec3 L;			 //incident direction
};


class PhotonMap : public RayTracer
{
  public:
	PhotonMap( Scene *scene, int maxDepth ) : RayTracer( scene, maxDepth ) { photonEmittanceLight( ray); } //correct arguments??
	void push( photon i );
	bool RussianRoulette( color &mCol );
	void photonEmittanceLight( Ray ray );
	void photonEmittance( RayHit hit, photon photon, Ray ray );
	void SampleP( Ray &ray, RayHit &hit, photon photon );
	color Sample( Ray &ray, RayHit &hit );
    color photonDensity( Ray &ray, RayHit hit, color BRDF );
	Ray ray;
	

};

