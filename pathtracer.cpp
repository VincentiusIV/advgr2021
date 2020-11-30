#include "precomp.h"

color PathTracer::Sample(Ray ray, Scene *scene) //added scene
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
			return mCol;
		}
		if ( mmat == MaterialType::MIRROR )
		{
			Ray r( hit.point, reflect( ray.direction, hit.normal ), INFINITY, ray.depth + 1 );
			return mCol * Sample( r, scene );
		}
		/*
		switch ( mat ->materialType)
		{
			case MaterialType::EMISSIVE: //return color of light source
				return mCol; // emittance == Material color? As you will give the light also a color...???  
			case MaterialType::MIRROR: Ray r( hit.point, reflect(ray.direction, hit.normal ), INFINITY, ray.depth + 1 );  //new ray, start at intersection point
				return mCol * Sample(r, scene); //albedo -> color of the material
			//default:
				//return Color( 0, 0, 0 );
		}*/
		
		//continue in random direction
		vec3 R = normalize(RandomInsideUnitSphere() + hit.normal); //get a random ray in random direction (DiffuseReflection). times normal???
		BRDF = mCol	/ PI; //albedo -> color of the material

		if ( ray.depth > maxDepth )
			return BRDF;
	
		//new ray, start at intersection point, into direction R
		Ray r( hit.point, R, INFINITY, ray.depth + 1 ); //wat doet ray depth + 1.0?

		//update throughout (recursion)
		color Ei = Sample( r, scene ) * ( dot( hit.normal, R ) ); //irradiance is what you found with that new ray
		return PI * 2.0f * BRDF * Ei;


	}
	else //no hit, ray left the scene. return black
	{
		return color( 0, 0, 0 );
	}


}


bool PathTracer::Trace(Scene *scene, Ray ray, RayHit &hit ) // why do we need scene? //why does heritage not work? and why does the raytracer do that?
{
	bool hitAny = false;
	for ( size_t i = 0; i < scene->objects.size(); i++ )
	{
		shared_ptr<HittableObject> obj = scene->objects.at( i );
		hitAny |= ( obj->Hit( ray, hit ) );
	}
	return hitAny;
}
