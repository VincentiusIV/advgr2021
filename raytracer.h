#pragma once

class Ray;
class RayHit;
class Scene;
class Material;

class RayTracer
{
  public:
	RayTracer( Scene *scene, int maxDepth ) : scene(scene), maxDepth( maxDepth ) {}
	virtual color Sample( Ray &ray ) = 0;
	virtual bool Trace( Ray &ray, RayHit &hit ) = 0;
	Scene *scene;
	int maxDepth;
};

class WhittedRayTracer : public RayTracer
{
  public:
	WhittedRayTracer( Scene *scene, int maxDepth ) : RayTracer( scene, maxDepth ) {}

	color Sample( Ray &ray );
	color DirectIllumination( point3 point, vec3 normal );
	bool Trace( Ray &ray, RayHit &hit );
	const color &HandleSkybox( Ray &ray );
	const color &HandleNormalTestMaterial( RayHit &hit );
	const color &HandleUVTestMaterial( RayHit &hit );
	const color &HandleDielectricMaterial( Ray &ray, RayHit &hit );
	const color &HandleGlassMaterial( Ray &ray, RayHit &hit );
	const color &HandleDiffuseMaterial( std::shared_ptr<Material> &mat, RayHit &hit );
	const color &HandleMirrorMaterial( RayHit &hit, Ray &ray );
	void Fresnel( float sinTheta, float &reflectance, float &cosi, float etat, float etai );

	color baseIllumination = color( 0.05f, 0.05f, 0.05f );
};