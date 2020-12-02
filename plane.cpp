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
			point3 I = ray.At( t );
			vec3 toIntersation = ( I - ray.origin );
			if ( toIntersation.sqrLentgh() > ray.tMax )
				return false;
			ray.t = t;
			hit.material = material;
			hit.point = I;
			hit.isFrontFace = denom <= 0.0;
			hit.normal = hit.isFrontFace ? planeNormal : -planeNormal;

			vec3 a = cross( planeNormal, vec3( 1, 0, 0 ) );
			vec3 b = cross( planeNormal, vec3( 0, 1, 0 ) );
			vec3 maxAB = dot( a, a ) < dot( b, b ) ? b : a;
			vec3 c = cross( planeNormal, vec3( 0, 0, 1 ) );
			vec3 u = normalize( dot(maxAB, maxAB) < dot(c,c) ? c : maxAB );
			vec3 v = cross( planeNormal, u );
			hit.uv.x = u.dot( hit.point ) / material->uvScale.x;
			hit.uv.y = v.dot( hit.point ) / material->uvScale.x;
			return true; 
		}
	}
	return false;
}
	
