#include "precomp.h"

color RayTracer::Sample( Ray &ray )
{
	RayHit hit;
	return Sample( ray, hit );
}

bool RayTracer::Trace( Ray &ray, RayHit &hit )
{
	return Trace( ray, hit, MaterialType::DIFFUSE );
}

bool RayTracer::Trace( Ray &ray, RayHit &hit, MaterialType typeToIgnore )
{
	// Bruteforce method
	bool hitAny = false;
	for ( size_t i = 0; i < scene->objects.size(); i++ )
	{
		shared_ptr<HittableObject> obj = scene->objects.at( i );
		if ( typeToIgnore != MaterialType::DIFFUSE && obj->material->materialType == typeToIgnore )
			continue;
		if (obj->Hit(ray, hit))
		{
			hit.hitObject = obj;
			hitAny |= true;
		}
	}

	hit.intersectsVolume = scene->volumes.size() > 0;
	if (hit.intersectsVolume)
	{
		shared_ptr<HittableObject> volume = scene->volumes.at( 0 );
		hit.volume = volume;
	}

	return hitAny;

	// TODO: Implement typeToIgnore with bvh.
	return scene->bvh->Intersect(ray, hit);
}

color WhittedRayTracer::DirectIllumination( point3 point, vec3 normal )
{
	color illumination = baseIllumination;
	for ( size_t i = 0; i < scene->lights.size(); i++ )
	{
		shared_ptr<Light> light = scene->lights.at( i );
		Ray shadowRay = light->CastShadowRayFrom(point);
		RayHit hit;
		if (Trace(shadowRay, hit, MaterialType::EMISSIVE))
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

color WhittedRayTracer::Sample( Ray &ray, RayHit &hit )
{
	// Ignore emissive objects (area lights), these are not supported.
	if (Trace(ray, hit, MaterialType::EMISSIVE))
	{
		shared_ptr<Material> mat = hit.material;
		if ( ray.depth >= maxDepth )
			return mat->GetColor(hit.uv);

		switch ( mat->materialType )
		{
			case MaterialType::DIFFUSE:
				return HandleDiffuseMaterial( mat, hit );
			case MaterialType::MIRROR:
				return HandleMirrorMaterial( ray,hit );
			case MaterialType::GLASS:
				return HandleGlassMaterial( ray, hit );
			case MaterialType::DIELECTRIC:
				return HandleDielectricMaterial( ray, hit );
			case MaterialType::NORMAL_TEST:
				return HandleNormalTestMaterial( hit );
			case MaterialType::UV_TEST:
				return HandleUVTestMaterial( hit );
			default:
				return color( 0.0, 0.0, 0.0 );
		}
	}
	else
	{
		if (useSkybox)
			return HandleSkybox( ray );
		else
			return color( 0.0, 0.0, 0.0 );
	}
}

const color &WhittedRayTracer::HandleSkybox( Ray &ray )
{
	vec3 unit_direction = ray.direction;
	auto t = 0.5 * ( -unit_direction.y + 1.0 );
	color c = ( 1.0 - t ) * color( 1.0, 1.0, 1.0 ) + t * color( 0.5, 0.7, 1.0 );
	return c;
}

const color &WhittedRayTracer::HandleNormalTestMaterial( RayHit &hit )
{
	return 0.5 * color( hit.normal.x + 1.0, hit.normal.y + 1.0, hit.normal.z + 1.0 ) * DirectIllumination( hit.point, hit.normal );
}

const color &WhittedRayTracer::HandleUVTestMaterial( RayHit &hit )
{
	return color(hit.uv.x, hit.uv.y, 1.0) * DirectIllumination( hit.point, hit.normal );
}

const color &RayTracer::HandleDielectricMaterial( Ray &ray, RayHit &hit )
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
	float sinTheta = etaiOverEtat * sqrtf( max( 0.0, 1.0 - cosTheta2 ) );
	float reflectance;
	Fresnel( sinTheta, reflectance, cosi, etat, etai );
	float transmittance = ( 1.0f - reflectance );
	float reflectChance = reflectance * Rand( 1.0f );
	float refractChance = transmittance * Rand( 1.0f );
	vec3 dir = k < 0 ? 0.0f : etaiOverEtat * ray.direction + ( etaiOverEtat * cosi - sqrtf( k ) ) * n;
	point3 refractRayOrigin = hit.isFrontFace ? hit.point - hit.normal * 0.001 : hit.point + hit.normal * 0.001f;
		
	Ray reflectRay( hit.point, reflect( ray.direction, hit.normal ), INFINITY, ray.depth + 1 );
	color refractColor = Sample( Ray( refractRayOrigin, dir, INFINITY, ray.depth + 1.0 ) );
	color reflectColor = Sample( reflectRay );
	return transmittance * refractColor + reflectance * reflectColor;

	if ( reflectChance > refractChance )
	{
		//Ray reflectRay( hit.point, reflect( ray.direction, hit.normal ), INFINITY, ray.depth + 1 );
		return Sample( reflectRay );
	}
	else
	{
		//vec3 dir = k < 0 ? 0.0f : etaiOverEtat * ray.direction + ( etaiOverEtat * cosi - sqrtf( k ) ) * n;
		//point3 refractRayOrigin = hit.isFrontFace ? hit.point - hit.normal * 0.001 : hit.point + hit.normal * 0.001f;
		return Sample( Ray( refractRayOrigin, dir, INFINITY, ray.depth + 1.0 ) );
	}

	/*
	color refractColor = Sample( Ray( refractRayOrigin, dir, INFINITY, ray.depth + 1.0 ), scene );
	color reflectColor = Sample( reflectRay, scene );
	return transmittance * refractColor + reflectance * reflectColor;
	*/
}

const color &WhittedRayTracer::HandleGlassMaterial( Ray &ray, RayHit &hit )
{
	vec3 dir = refract( ray.direction, hit.normal, hit.material->n );
	point3 refractRayOrigin = hit.isFrontFace ? hit.point - hit.normal * 0.001f : hit.point + hit.normal * 0.001f;
	return RayTracer::Sample( Ray( refractRayOrigin, dir, INFINITY, ray.depth + 1 ) );
}

const color &WhittedRayTracer::HandleDiffuseMaterial( std::shared_ptr<Material> &mat, RayHit &hit )
{
	return mat->GetColor(hit.uv) * DirectIllumination( hit.point, hit.normal );
}

const color &WhittedRayTracer::HandleMirrorMaterial( Ray &ray, RayHit &hit )
{
	point3 p = hit.point;
	vec3 r = reflect( ray.direction, hit.normal );
	Ray reflectRay( p, r + ( 1.0f - hit.material->smoothness ) * RandomInsideUnitSphere(), INFINITY, ray.depth + 1 );
	color reflectColor = RayTracer::Sample( reflectRay );
	color diffuseColor = ( 1.0 - hit.material->specularity ) * hit.material->GetColor(hit.uv) * DirectIllumination( hit.point, hit.normal );
	return  diffuseColor + ( hit.material->specularity * reflectColor );
}

void RayTracer::Fresnel( float sinTheta, float &reflectance, float &cosi, float etat, float etai )
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
