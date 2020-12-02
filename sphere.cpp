#include "precomp.h"

Sphere::Sphere( shared_ptr<Material> material, float radius ) : HittableObject( material ), r( radius ), r2( radius * radius )
{

}

bool Sphere::Hit( Ray &ray, RayHit &hit )
{
	float t;
	if (Sphere::Hit(ray, hit, position, r2, t))
	{
		ray.t = t;
		hit.material = material;
		hit.point = ray.At( t );
		vec3 outNormal = GetNormalAtPoint(hit.point);
		hit.isFrontFace = dot( ray.direction, outNormal ) <= 0.0;
		hit.normal = hit.isFrontFace ? outNormal : -outNormal;
		hit.uv.x = 0.5 + atan2( hit.normal.z, hit.normal.x ) / ( 2 * PI );
		hit.uv.y = 0.5 - asin( hit.normal.y ) / PI;
		return true;
	}
	return false;
}

bool Sphere::Hit( Ray &ray, RayHit &hit, point3 spherePos, float radius )
{
	float t;
	return Sphere::Hit(ray, hit, spherePos, radius, t);
}

bool Sphere::Hit( Ray &ray, RayHit &hit, point3 spherePos, float r2, float &t )
{
	vec3 C = spherePos - ray.origin;
	t = dot( C, ray.direction );
	vec3 Q = C - t * ray.direction;
	float p2 = dot( Q, Q );
	if ( p2 > r2 )
		return false;
	t -= sqrt( r2 - p2 );
	if ( ( t < ray.t ) && ( t > 0 ) )
	{
		point3 intersection = ray.At( t );
		if ( ( intersection - ray.origin ).sqrLentgh() > ray.tMax )
			return false;
		return true;
	}
	else
	{
		return false;
	}
}

point3 Sphere::GetRandomPoint()
{
	return position + RandomInsideUnitSphere() * r;
}

vec3 Sphere::GetNormalAtPoint( const point3 &point )
{
	return normalize( point - position );
}
