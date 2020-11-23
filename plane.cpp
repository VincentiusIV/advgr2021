#include "precomp.h"

Plane::Plane( shared_ptr<Material> material, vec3 normal ) : HittableObject( material ), normalP ( normal )
{

}

bool Plane::Hit( Ray &ray, RayHit &hit )
{
	// Plane: P . N(vec) + d = 0

	float denom = dot( normalP, ray.direction );
	if (fabs(denom) > 0.000001f) // bigger than 0
	{
		vec3 p0l0 = ( position - ray.origin );
		float t = dot( p0l0, normalP ) / (denom);
		if ((t < ray.t) && (t > 0.0001f) )
		{
			Point3 I = ray.At( t );
			if ( ( I - ray.origin ).sqrLentgh() > ray.tMax )
				return false;

			ray.t = t;
			hit.material = material;
			hit.point = I;
			hit.isFrontFace = denom <= 0.0;
			hit.normal = hit.isFrontFace ? normalP : -normalP;
			return true; 
		}
	}
	return false;
    
	//rayclass can use the t for ray position  

}
	
