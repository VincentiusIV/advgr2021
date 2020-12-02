#include "precomp.h"

color PathTracer::Sample(Ray &ray)
{
	//initials
	color BRDF; //should probably be outside function
	RayHit hit;
	if ( ray.depth > maxDepth ) //reasonable number = 7
		return color(0,0,0);
	if (Trace(ray, hit))
	{
		shared_ptr<Material> mat = hit.material;
		color mCol = mat->color;
		MaterialType mmat = mat->materialType;

		if ( mmat == MaterialType::EMISSIVE )
		{
			return mCol; //return color of light source. This will be the 'material' color.
		}
		if ( mmat == MaterialType::MIRROR )
		{
			Ray r( hit.point, reflect( ray.direction, hit.normal ) + ( 1.0f - hit.material->smoothness ) * RandomInsideUnitSphere(), INFINITY, ray.depth + 1 ); //new ray from intersection point
			return  Sample( r ); //Color of the material -> Albedo
		}
		if (mmat == MaterialType::DIELECTRIC || mmat == MaterialType::GLASS)
		{
			return HandleDielectricMaterial( ray, hit );
		}


		BRDF = mCol / PI;
			return LitMethod1( ray, hit, BRDF );
		/*if ( Rand( 1.0 ) > 0.5f )
			return LitMethod1( ray, hit, BRDF );
		else 
			return LitMethod2( ray, hit, BRDF );*/
	}
	return color( 0, 0, 0 );
}

const color &PathTracer::LitMethod1( Ray &ray, RayHit &hit, color &BRDF )
{
	vec3 R = RandomInsideUnitSphere();
	if ( R.dot( hit.normal ) < 0.0 )
		R = -R;
	R = normalize( R );
	//new ray, start at intersection point, into random direction R
	point3 o = hit.point + hit.normal * EPSILON;
	Ray r( o, R, INFINITY, ray.depth + 1 );
	float ir = dot( hit.normal, R );
	color Ei = Sample( r ) * ( ir ); //irradiance is what you found with that new ray
	return TWO_PI * BRDF * Ei;
}

const color &PathTracer::LitMethod2( Ray &ray, RayHit &hit, color &BRDF )
{
	shared_ptr<HittableObject> randLight = scene->GetRandomEmissiveObject();
	point3 randPoint = randLight->GetRandomPoint();
	vec3 L = randPoint - hit.point;
	float dist = L.length();
	L = L / dist;
	vec3 lightNormal = randLight->GetNormalAtPoint( randPoint );
	float cosO = ( -L ).dot( lightNormal );
	float cosI = L.dot( hit.normal );
	if ( ( cosO <= 0 ) || ( cosI <= 0 ) )
		return color( 0, 0, 0 );
	point3 o = hit.point + L * EPSILON;
	float tmax = dist - 2.0f * EPSILON;
	Ray r( o, L, tmax, 7 );
	if ( !Trace( r, hit, MaterialType::EMISSIVE) )
	{
		float solidAngle = ( cosO * randLight->GetArea() / ( dist * dist ) );
		return BRDF * (double)scene->emissiveObjects.size() * randLight->material->color * solidAngle * cosI;
	}
	else
	{
		return color( 0, 0, 0 );
	}
}

