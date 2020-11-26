#include "precomp.h"

Plane::Plane( shared_ptr<Material> material, vec3 normal ) : HittableObject( material ), planeNormal( normal ), width( INFINITY ), height( INFINITY )
{

}

Plane::Plane( shared_ptr<Material> material, vec3 normal, float width, float height ) : HittableObject( material ), planeNormal( normal ), width( width ), height( height )
{

}

bool Plane::Hit( Ray &ray, RayHit &hit )
{
	float denom = dot( planeNormal, ray.direction );
	if (fabs(denom) > 0.000001f)
	{
		vec3 p0l0 = ( position - ray.origin );
		float t = dot( p0l0, planeNormal ) / (denom);
		if ((t < ray.t) && (t > 0.0001f) )
		{
			Point3 I = ray.At( t );
			if ( ( I - ray.origin ).sqrLentgh() > ray.tMax )
				return false;

			ray.t = t;
			hit.material = material;
			hit.point = I;
			hit.isFrontFace = denom <= 0.0;
			hit.normal = hit.isFrontFace ? planeNormal : -planeNormal;
			return true; 
		}
	}
	return false;
}
	
