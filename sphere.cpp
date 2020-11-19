#include "precomp.h"

Sphere::Sphere( shared_ptr<Material> material, float radius ) : HittableObject( material ), r( radius ), r2( r * r )
{

}

bool Sphere::Hit( Ray &ray, RayHit &hit )
{
	vec3 C = position - ray.origin;
	float t = dot( C, ray.direction );
	vec3 Q = C - t * ray.direction;
	float p2 = dot( Q, Q );
	if ( p2 > r2 )
		return false;
	t -= sqrt( r2 - p2 );
	if ( ( t < ray.t ) && ( t > 0 ) )
	{
		ray.t = t;
		hit.material = material;
		hit.point = ray.At( t );
		hit.normal = normalize( hit.point - position );
		return true;
	}
	else
	{
		return false;
	}
}
