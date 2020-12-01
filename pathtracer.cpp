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
		{
			Ray r( hit.point, reflect( ray.direction, hit.normal ), INFINITY, ray.depth + 1 ); //new ray from intersection point
			return mCol * Sample( r, scene ); //Color of the material -> Albedo
		}
		/*if (mmat == MaterialType::DIELECTRIC)
		{
		 ex fresnel = 0.1 -> 0.1 chance for reflection and 0.9 chance for refraction (etc)
		}*/
		

		BRDF = mCol / PI; //albedo -> color of the material

		if ( ray.depth > maxDepth ) //reasonable number = 7
			return color(0,0,0);

		//continue in random direction on your hemisphere
		vec3 randomDeviation;//		= RandomInsideUnitSphere(); 
		do
		{
			randomDeviation = RandomInsideUnitSphere();
		
		} while ( randomDeviation.dot( hit.normal ) < 0.00001);
		vec3 R =  randomDeviation; //get a random ray in random direction (DiffuseReflection)
		


		//new ray, start at intersection point, into random direction R
		Ray r( hit.point + hit.normal * 0.001f, R, INFINITY, ray.depth + 1 ); 
		

		float ir = dot( hit.normal, R );
		//ir = fmin( ir, 0.3f );

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
