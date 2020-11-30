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

		}*/
		
		//continue in random direction
		vec3 R = normalize(RandomInsideUnitSphere() + hit.normal); //get a random ray in random direction (DiffuseReflection)
		BRDF = mCol	/ PI; //Color of the material -> Albedo

		if ( ray.depth > maxDepth ) 
			return BRDF;
	
		//new ray, start at intersection point, into random direction R
		Ray r( hit.point, R, INFINITY, ray.depth + 1 ); 

		//update throughout, (recursion)
		color Ei = Sample( r, scene ) * ( dot( hit.normal, R ) ); //irradiance is what you found with that new ray
		return (PI * 2.0f * BRDF * Ei);

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
