#pragma once

class Ray;
class RayHit;
class Scene;
class Material;
enum class MaterialType;

class RayTracer
{
  public:
	RayTracer( Scene *scene, int maxDepth ) : scene(scene), maxDepth( maxDepth ) {}
	color Sample( Ray &ray );
	virtual color Sample( Ray &ray, RayHit &hit ) = 0;
	bool Trace( Ray &ray, RayHit &hit );
	bool Trace( Ray &ray, RayHit &hit, MaterialType typeToIgnore ); 
	void Fresnel( float sinTheta, float &reflectance, float &cosi, float etat, float etai );
	const color &HandleDielectricMaterial( Ray &ray, RayHit &hit);
	Scene *scene;
	int maxDepth;
};

class WhittedRayTracer : public RayTracer
{
  public:
	WhittedRayTracer( Scene *scene, int maxDepth ) : RayTracer( scene, maxDepth ) {}

	color Sample( Ray &ray, RayHit &hit );
	color DirectIllumination( point3 point, vec3 normal );
	const color &HandleSkybox( Ray &ray );
	const color &HandleNormalTestMaterial( RayHit &hit );
	const color &HandleUVTestMaterial( RayHit &hit );
	const color &HandleGlassMaterial( Ray &ray, RayHit &hit );
	const color &HandleDiffuseMaterial( std::shared_ptr<Material> &mat, RayHit &hit );
	const color &HandleMirrorMaterial( Ray &ray, RayHit &hit );

	color baseIllumination = color( 0.05f, 0.05f, 0.05f );
};