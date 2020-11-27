#include "precomp.h"

bool WhittedRayTracer::Trace( Scene *scene, Ray ray, RayHit& hit)
{
	bool hitAny = false;
	for ( size_t i = 0; i < scene->objects.size(); i++ )
	{
		shared_ptr<HittableObject> obj = scene->objects.at( i );
		hitAny |= ( obj->Hit( ray, hit ) );
	}
	return hitAny;
}

color WhittedRayTracer::DirectIllumination( Scene *scene, point3 point, vec3 normal )
{
	color illumination = baseIllumination;
	for ( size_t i = 0; i < scene->lights.size(); i++ )
	{
		shared_ptr<Light> light = scene->lights.at( i );
		Ray shadowRay = light->CastShadowRayFrom(point);
		RayHit hit;
		if (Trace(scene, shadowRay, hit))
		{
			continue;
		}
		else
		{
			illumination += light->Illuminate( point, normal, shadowRay );
		}
	}
	illumination.x = clamp( illumination.x, 0.0f, 1.0f );
	illumination.y = clamp( illumination.y, 0.0f, 1.0f );
	illumination.z = clamp( illumination.z, 0.0f, 1.0f );
	return illumination;
}

vec3 RandomInsideUnitSphere() { return vec3( -1.0f + Rand( 2.0f ), -1.0f + Rand( 2.0f ), -1.0f + Rand( 2.0f ) ); }

color WhittedRayTracer::Sample( Ray ray, Scene *scene )
{
	RayHit hit;
	if (Trace(scene, ray, hit))
	{
		shared_ptr<Material> mat = hit.material;
		if ( ray.depth >= maxDepth )
			return mat->color;

		switch ( mat->materialType )
		{
			case MaterialType::DIFFUSE:
			case MaterialType::EMISSIVE:
				return HandleDiffuseMaterial( mat, scene, hit );
			case MaterialType::MIRROR:
				return HandleMirrorMaterial( hit, ray, scene );
			case MaterialType::GLASS:
				return HandleGlassMaterial( ray, hit, scene );
			case MaterialType::DIELECTRIC:
				return HandleDielectricMaterial( ray, hit, scene );
			case MaterialType::NORMAL_TEST:
				return HandleNormalTestMaterial( hit, scene );
			default:
				return color( 0.0, 0.0, 0.0 );
		}
	}
	else
	{
		return HandleSkybox( ray );
	}
}

const color &WhittedRayTracer::HandleSkybox( Ray &ray )
{
	vec3 unit_direction = ray.direction;
	auto t = 0.5 * ( -unit_direction.y + 1.0 );
	color c = ( 1.0 - t ) * color( 1.0, 1.0, 1.0 ) + t * color( 0.5, 0.7, 1.0 );
	return c;
}

const color &WhittedRayTracer::HandleNormalTestMaterial( RayHit &hit, Scene *scene )
{
	return 0.5 * color( hit.normal.x + 1.0, hit.normal.y + 1.0, hit.normal.z + 1.0 ) * DirectIllumination( scene, hit.point, hit.normal );
}

const color &WhittedRayTracer::HandleDielectricMaterial( Ray &ray, RayHit &hit, Scene *scene )
{
	float cosi = fmax( -1.0, fmin( 1.0, dot( ray.direction, hit.normal ) ) );
	float cosTheta2 = cosi * cosi;
	float etai = 1.0, etat = hit.material->n;
	vec3 n = hit.normal;
	if ( cosi < 0 )
	{
		cosi = -cosi;
	}
	else
	{
		std::swap( etai, etat );
		n = -hit.normal;
	}
	float etaiOverEtat = etai / etat;
	float k = 1.0f - etaiOverEtat * etaiOverEtat * ( 1.0 - cosTheta2 );
	vec3 dir = k < 0 ? 0.0f : etaiOverEtat * ray.direction + ( etaiOverEtat * cosi - sqrtf( k ) ) * n;
	point3 refractRayOrigin = hit.isFrontFace ? hit.point - hit.normal * 0.001 : hit.point + hit.normal * 0.001f;

	// Fresnel
	float sinTheta = etaiOverEtat * sqrtf( max( 0.0, 1.0 - cosTheta2 ) );
	float reflectance;
	Fresnel( sinTheta, reflectance, cosi, etat, etai );
	float transmittance = ( 1.0f - reflectance );

	point3 p = hit.point;
	vec3 r = reflect( ray.direction, hit.normal );
	Ray reflectRay( p, r + ( 1.0f - hit.material->smoothness ) * RandomInsideUnitSphere(), INFINITY, ray.depth + 1 );
	color refractColor = Sample( Ray( refractRayOrigin, dir, INFINITY, ray.depth + 1.0 ), scene );
	color reflectColor = Sample( reflectRay, scene );
	return transmittance * refractColor + reflectance * reflectColor;
}

const color &WhittedRayTracer::HandleGlassMaterial( Ray &ray, RayHit &hit, Scene *scene )
{
	vec3 dir = refract( ray.direction, hit.normal, hit.material->n );
	point3 refractRayOrigin = hit.isFrontFace ? hit.point - hit.normal * 0.001f : hit.point + hit.normal * 0.001f;
	return Sample( Ray( refractRayOrigin, dir, INFINITY, ray.depth + 1 ), scene );
}

const color &WhittedRayTracer::HandleDiffuseMaterial( std::shared_ptr<Material> &mat, Scene *scene, RayHit &hit )
{
	return mat->color * DirectIllumination( scene, hit.point, hit.normal );
}

const color &WhittedRayTracer::HandleMirrorMaterial( RayHit &hit, Ray &ray, Scene *scene )
{
	point3 p = hit.point;
	vec3 r = reflect( ray.direction, hit.normal );
	Ray reflectRay( p, r + ( 1.0f - hit.material->smoothness ) * RandomInsideUnitSphere(), INFINITY, ray.depth + 1 );
	color reflectColor = Sample( reflectRay, scene );
	color diffuseColor = ( 1.0 - hit.material->specularity ) * hit.material->color * DirectIllumination( scene, hit.point, hit.normal );
	return  diffuseColor + ( hit.material->specularity * reflectColor );
}

void WhittedRayTracer::Fresnel( float sinTheta, float &reflectance, float &cosi, float etat, float etai )
{
	if ( sinTheta >= 1.f )
	{
		reflectance = 1.0f;
	}
	else
	{
		float cost = sqrtf( max( 0.0f, 1.0f - sinTheta * sinTheta ) );
		cosi = fabsf( cosi );
		float rs = ( ( etat * cosi ) - ( etai * cost ) ) / ( ( etat * cosi ) + ( etai * cost ) );
		float rp = ( ( etai * cosi ) - ( etat * cost ) ) / ( ( etai * cosi ) + ( etat * cost ) );
		reflectance = 0.5f * ( rs * rs + rp * rp );
	}
}
