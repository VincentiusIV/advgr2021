#pragma once

class Ray;
class RayHit;
class Scene;
class Material;

class RayTracer
{
public:
	virtual color Sample( Ray ray, Scene *scene ) = 0;
	virtual bool Trace( Scene *scene, Ray ray, RayHit &hit ) = 0;
};

class WhittedRayTracer : public RayTracer
{
  public:
	WhittedRayTracer(int maxDepth) : maxDepth(maxDepth) {}

	color Sample( Ray ray, Scene *scene );
	color DirectIllumination( Scene *scene, point3 point, vec3 normal );
	bool Trace( Scene *scene, Ray ray, RayHit &hit );
	const color &HandleSkybox( Ray &ray );
	const color &HandleNormalTestMaterial( RayHit &hit, Scene *scene );
	const color &HandleUVTestMaterial( RayHit &hit, Scene *scene );
	const color &HandleDielectricMaterial( Ray &ray, RayHit &hit, Scene *scene );
	const color &HandleGlassMaterial( Ray &ray, RayHit &hit, Scene *scene );
	const color &HandleDiffuseMaterial( std::shared_ptr<Material> &mat, Scene *scene, RayHit &hit );
	const color &HandleMirrorMaterial( RayHit &hit, Ray &ray, Scene *scene );
	void Fresnel( float sinTheta, float &reflectance, float &cosi, float etat, float etai );
	int maxDepth;
	color baseIllumination = color( 0.05f, 0.05f, 0.05f );
};