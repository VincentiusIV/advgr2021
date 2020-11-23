#include "precomp.h"

Plane::Plane( shared_ptr<Material> material, vec3 normal ) : HittableObject( material ), normalP ( normal )
{

}

bool Plane::Hit( Ray &ray, RayHit &hit )
{
	// Plane: P . N(vec) + d = 0

	float denom = dot( normalP, ray.direction );

	if (fabs(denom) > 0.0001f) // bigger than 0
	{
		vec3 normray = ( position - ray.origin );
		float numer = dot( normalP, normray );
		float t = ( numer / denom );
		if ((t < ray.t) && (t >= 0.00001f) )
		{
			ray.t = t;
			hit.material = material;
			hit.point = ray.At( t );
			hit.normal = normalP;
			return true; 
		}
	}
	return false;
    
	//rayclass can use the t for ray position  

}
	
