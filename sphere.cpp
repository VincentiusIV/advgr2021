#include "precomp.h"

Sphere::Sphere( shared_ptr<Material> material, float radius ) : HittableObject( material ), r( radius ), r2( radius * radius )
{

}

bool TestHit( Ray &ray, RayHit &hit, point3 spherePos, float r2, float &t )
{
	vec3 C = spherePos - ray.origin;
	t = dot( C, ray.direction );
	vec3 Q = C - t * ray.direction;
	float p2 = dot( Q, Q );
	if ( p2 > r2 )
		return false;
	float b = t;
	float det = sqrt( r2 - p2 );
	t -= det;
	return  (( t < ray.t ) && ( t > 0 ) && t < ray.tMax );
}

bool Sphere::Hit( Ray &ray, RayHit &hit )
{
	float t;
	if ( TestHit( ray, hit, position, r2, t ) )
	{
		ray.t = t;
		hit.material = material;
		hit.point = ray.At( t );
		vec3 outNormal = GetNormalAtPoint( hit.point );
		hit.isFrontFace = dot( ray.direction, outNormal ) <= 0.0;
		hit.normal = hit.isFrontFace ? outNormal : -outNormal;
		hit.uv.x = 0.5 + atan2( hit.normal.z, hit.normal.x ) / ( 2 * PI );
		hit.uv.y = 0.5 - asin( hit.normal.y ) / PI;
		return true;
	}
	return false;
}
point3 Sphere::GetRandomPoint()
{
	vec3 randPoint = RandomInsideUnitSphere() * r;
	return position + randPoint;
}

vec3 Sphere::GetNormalAtPoint( const point3 &point )
{
	return normalize( point - position );
}

void Sphere::UpdateAABB()
{
	aabb.min = position - vec3( r, r, r );
	aabb.max = position + vec3( r, r, r );
}
