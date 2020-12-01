#include "precomp.h"

color PathTracer::Sample(Ray ray, Scene *scene)
{
	//initials
	color BRDF; //should probably be outside function
	RayHit hit;

	if (Trace(scene, ray, hit))
	{
		shared_ptr<Material> mat = hit.material;
		color mCol = mat->color;
		MaterialType mmat = mat->materialType;

		if ( mmat == MaterialType::EMISSIVE )
		{
			return mCol; //return color of light source. This will be the 'material' color.
		}
		if ( mmat == MaterialType::MIRROR )
		{   //+(1.0f - hit.material->smoothness)
			Ray r( hit.point, (reflect( ray.direction, hit.normal )), INFINITY, ray.depth + 1 ); //new ray from intersection point
			return mCol * Sample( r, scene ); //Color of the material -> Albedo
		}
		if (mmat == MaterialType::DIELECTRIC || mmat == MaterialType::GLASS)
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

			//only different code than in the witted raytracer.
			if (reflectChance > refractChance)
			{
				Ray reflectRay( hit.point, reflect( ray.direction, hit.normal ) + ( 1.0f - hit.material->smoothness ) * RandomInsideUnitSphere(), INFINITY, ray.depth + 1 );
				return Sample( reflectRay, scene );
			}
			else
			{
				vec3 dir = k < 0 ? 0.0f : etaiOverEtat * ray.direction + ( etaiOverEtat * cosi - sqrtf( k ) ) * n;
				point3 refractRayOrigin = hit.isFrontFace ? hit.point - hit.normal * 0.001 : hit.point + hit.normal * 0.001f;
				return Sample( Ray( refractRayOrigin, dir, INFINITY, ray.depth + 1.0 ), scene );
			}
		}
		
		BRDF = mCol / PI; //albedo -> color of the material

		if ( ray.depth > maxDepth ) //reasonable number = 7
			return color(0,0,0);

		//continue in random direction on your hemisphere
		vec3 randomDeviation;
		do
		{
			randomDeviation = RandomInsideUnitSphere();
		
		} while ( randomDeviation.dot( hit.normal ) < 0.00001);
		vec3 R =  randomDeviation; //get a random ray in random direction (DiffuseReflection)
		

		//new ray, start at intersection point, into random direction R
		Ray r( hit.point + hit.normal * 0.001f, R, INFINITY, ray.depth + 1 ); 
		

		float ir = dot( hit.normal, R );
		
		//update throughout (recursion)
		color Ei = Sample( r, scene ) * (ir); //irradiance is what you found with that new ray
		return PI * 2.0f * BRDF * Ei;
	}
	else //no hit, ray left the scene. return black
	{
		return color( 0, 0, 0 );
	}
}

bool PathTracer::Trace(Scene *scene, Ray ray, RayHit &hit ) 
{
	bool hitAny = false;
	for ( size_t i = 0; i < scene->objects.size(); i++ )
	{
		shared_ptr<HittableObject> obj = scene->objects.at( i );
		hitAny |= ( obj->Hit( ray, hit ) );
	}
	return hitAny;
}

void PathTracer::Fresnel( float sinTheta, float &reflectance, float &cosi, float etat, float etai )
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
