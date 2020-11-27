#pragma once

class Ray;
class RayHit;
class Scene;
class Material;

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
	Color DirectIllumination( Scene *scene, Point3 point, vec3 normal );
	const Color &HandleSkybox( Ray &ray );
	const Color &HandleNormalTestMaterial( RayHit &hit, Scene *scene );
	const Color &HandleDielectricMaterial( Ray &ray, RayHit &hit, Scene *scene );
	const Color &HandleGlassMaterial( Ray &ray, RayHit &hit, Scene *scene );
	const Color &HandleDiffuseMaterial( std::shared_ptr<Material> &mat, Scene *scene, RayHit &hit );
	const Color &HandleMirrorMaterial( RayHit &hit, Ray &ray, Scene *scene );
	void Fresnel( float sinTheta, float &reflectance, float &cosi, float etat, float etai );
	int maxDepth;
	Color baseIllumination = Color( 0.05f, 0.05f, 0.05f );
};