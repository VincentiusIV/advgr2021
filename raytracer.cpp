#include "precomp.h"

bool NearestIntersection( Scene *scene, Ray ray, RayHit& hit)
{
	bool hitAny = false;
	for ( size_t i = 0; i < scene->objects.size(); i++ )
	{
		shared_ptr<HittableObject> obj = scene->objects.at( i );
		hitAny |= ( obj->Hit( ray, hit ) );
	}
	return hitAny;
}

float DirectIllumination(Scene* scene, Point3 point, Vector3 normal)
{
	float illumination = 0.0f;
	for ( size_t i = 0; i < scene->lights.size(); i++ )
	{
		shared_ptr<Light> light = scene->lights.at( i );
		Ray shadowRay = Ray(point, normalize(light->position - point), INFINITY, 0);
		RayHit hit;
		if (NearestIntersection(scene, shadowRay, hit))
		{
			continue;
		}
		else
		{
			float d = dot(normalize( normal), normalize(shadowRay.direction ));
			if (d >= 0)
				illumination += d;
			if ( illumination > 1 )
				illumination = 1;
		}
	}

	return illumination;
}

vec3 reflect( const vec3 &dir, const vec3 &normal ) { return ( dir - 2.0f * dot( dir, normal ) * normal ); }
vec3 refract( const vec3 &incomingDir, const vec3 &normal, const float &reflectionRatio )
{
	float cosTheta = fmax(-1.0, fmin( 1.0f, dot( incomingDir, normal ) ));
	float etai = 1.0f, etat = reflectionRatio;
	vec3 n = normal;
	if ( cosTheta < 0 ) 
	{ 
		cosTheta = -cosTheta; 
	}
	else
	{
		std::swap( etai, etat );
		n = -normal;
	}
	float etaiOverEtat = etai / etat;
	float k = 1.0f - etaiOverEtat * etaiOverEtat * ( 1.0f - cosTheta * cosTheta );
	return k < 0 ? 0.0f : etaiOverEtat * incomingDir + ( etaiOverEtat * cosTheta - sqrtf( k ) ) * n;
}

Color WhittedRayTracer::Trace( Ray ray, Scene *scene )
{
	RayHit hit;
	if (NearestIntersection(scene, ray, hit))
	{
		shared_ptr<Material> mat = hit.material;
		if ( ray.depth >= maxDepth )
			return mat->color;

		switch ( mat->materialType )
		{
			case MaterialType::DIFFUSE:
				return mat->color * DirectIllumination( scene, hit.point, hit.normal );
			case MaterialType::MIRROR:
			{
				Point3 p = hit.point;
				Vector3 r = reflect( ray.direction, hit.normal );
				Ray reflectRay( p, r, INFINITY, ray.depth+1 );
				return mat->color * Trace( reflectRay, scene );
			}
			case MaterialType::GLASS:
			{
				vec3 dir = refract( ray.direction, hit.normal, hit.material->n );
				Point3 refractRayOrigin = hit.isFrontFace ? hit.point - hit.normal * 0.001f : hit.point + hit.normal * 0.001f; 
				return Trace( Ray( refractRayOrigin, dir, INFINITY, ray.depth+1 ), scene );
			}
			case MaterialType::DIELECTRIC:
			{
				float cosi = fmax( -1.0, fmin( 1.0f, dot(ray.direction, hit.normal ) ) );
				float cosTheta2 = cosi * cosi;
				float etai = 1.0f, etat = hit.material->n;
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
				float k = 1.0f - etaiOverEtat * etaiOverEtat * ( 1.0f - cosTheta2 );
				vec3 dir = k < 0 ? 0.0f : etaiOverEtat * ray.direction + ( etaiOverEtat * cosi - sqrtf( k ) ) * n;
				Point3 refractRayOrigin = hit.isFrontFace ? hit.point - hit.normal * 0.001f : hit.point + hit.normal * 0.001f; 
				
				// Fresnel
				float sinTheta = etaiOverEtat * sqrtf( max( 0.0f, 1.0f - cosTheta2 ) );
				float reflectance;
				Fresnel( sinTheta, reflectance, cosi, etat, etai );
				float transmittance = ( 1.0f - reflectance );
				
				Point3 p = hit.point;
				Vector3 r = reflect( ray.direction, hit.normal );
				Ray reflectRay( p, r, INFINITY, ray.depth + 1 );
				return transmittance * Trace( Ray( refractRayOrigin, dir, INFINITY, ray.depth + 1 ), scene ) + reflectance * Trace( reflectRay, scene );
			}
			case MaterialType::NORMAL_TEST:
				return 0.5f * Color( hit.normal.x + 1, hit.normal.y + 1, hit.normal.z + 1 ) * DirectIllumination(scene, hit.point, hit.normal);
			default:
				return Color(0.0, 0.0, 0.0);
		}
	}
	else
	{
		vec3 unit_direction = ray.direction;
		auto t = 0.5 * ( -unit_direction.y + 1.0 );
		Color color = ( 1.0 - t ) * Color( 1.0, 1.0, 1.0 ) + t * Color( 0.5, 0.7, 1.0 );
		return color;
	}
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
