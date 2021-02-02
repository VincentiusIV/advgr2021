#include "precomp.h"

color PathTracer::Sample( Ray &ray, RayHit &hit )
{
	color BRDF;
	if ( ray.depth > maxDepth )
		return color(0,0,0);
	if (Trace(ray, hit))
	{
		shared_ptr<Material> mat = hit.material;
		color mCol = mat->GetColor(hit.uv);
		MaterialType mmat = mat->materialType;

		if ( mmat == MaterialType::EMISSIVE )
		{
			return mCol;
		}
		if ( mmat == MaterialType::MIRROR )
		{
			Ray r( hit.point, reflect( ray.direction, hit.normal ) + ( 1.0f - hit.material->smoothness ) * RandomInsideUnitSphere(), INFINITY, ray.depth + 1 ); //new ray from intersection point
			return (( 1.0 - hit.material->specularity ) * mCol) + ((hit.material->specularity) * RayTracer::Sample( r ));																					//Color of the material -> Albedo
		}
		if (mmat == MaterialType::DIELECTRIC || mmat == MaterialType::GLASS)
		{
			return HandleDielectricMaterial( ray, hit );
		}

		BRDF = mCol / PI;
		RayHit indirectHit, directHit;
		color indirectColor = IndirectIllumination( ray, hit, BRDF, indirectHit );
		color directColor = DirectIllumiation( ray, hit, BRDF, directHit);
		return ( directColor + indirectColor ) / 2;
	}
	//return color( 0, 0, 0 );
	vec3 unit_direction = ray.direction;
	auto t = 0.5 * ( -unit_direction.y + 1.0 );
	color c = ( 1.0 - t ) * color( 1.0, 1.0, 1.0 ) + t * color( 0.5, 0.7, 1.0 );
	return c;
}

const color &PathTracer::IndirectIllumination( Ray &ray, RayHit &hit, color &BRDF, RayHit &indirectHit )
{
	vec3 R = RandomInsideUnitSphere();
	if ( R.dot( hit.normal ) < 0.0 )
		R = -R;
	R = normalize( R );
	//new ray, start at intersection point, into random direction R
	point3 o = hit.point + hit.normal * EPSILON;
	Ray r( o, R, INFINITY, ray.depth + 1 );
	double ir = dot( hit.normal, R );
	color Ei = Sample( r, indirectHit ) * ( ir ); //irradiance is what you found with that new ray
	return TWO_PI * BRDF * Ei;
}

const color &PathTracer::DirectIllumiation( Ray &ray, RayHit &hit, color &BRDF, RayHit &directHit )
{
	shared_ptr<HittableObject> randLight = scene->GetRandomEmissiveObject();
	point3 randPoint = randLight->GetRandomPoint();
	vec3 L = randPoint - hit.point;
	float dist = L.length();
	L = normalize(L);
	vec3 lightNormal = randLight->GetNormalAtPoint( randPoint );
	float cosO = ( -L ).dot( lightNormal );
	float cosI = L.dot( hit.normal );
	if ( ( cosO <= 0 ) || ( cosI <= 0 ) )
		return color( 0, 0, 0 );
	bool hitSmth = false;
	point3 o = hit.point + L * EPSILON;
	float tmax = dist - 2.0f * EPSILON;
	Ray r( o, L, tmax, maxDepth );

	if ( !Trace( r, directHit, MaterialType::EMISSIVE ) )
	{
		float solidAngle = ( cosO * randLight->GetArea() / ( dist * dist ) );
		return BRDF * (double)scene->emissiveObjects.size() * randLight->material->albedo * solidAngle * cosI;
	}
	else
	{
		return color( 0, 0, 0 );
	}
}

