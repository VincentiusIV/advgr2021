#include "precomp.h"

bool AABB::Contains( const point3 &p )
{
	return p.x > min.x && p.x < max.x &&
		   p.y > min.y && p.y < max.y &&
		   p.z > min.z && p.z < max.z;
}

bool AABB::Intersect( Ray &r )
{
	vec3 invD = vec3( 1.0 / r.direction.x, 1.0 / r.direction.y, 1.0 / r.direction.z );
	vec3 t0s = ( min - r.origin ) * invD;
	vec3 t1s = ( max - r.origin ) * invD;

	vec3 tsmaller = vec3( fmin( t0s.x, t1s.x ), fmin( t0s.y, t1s.y ), fmin( t0s.z, t1s.z ) );
	vec3 tbigger = vec3( fmax( t0s.x, t1s.x ), fmax( t0s.y, t1s.y ), fmax( t0s.z, t1s.z ) );

	float tmin = fmax( 0.0f, fmax( tsmaller.x, fmax( tsmaller.y, tsmaller.z ) ) );
	float tmax = fmin( r.tMax, fmin( tbigger.x, fmin( tbigger.y, tbigger.z ) ) );
	if (tmin < tmax)
	{
		//r.tMax = tmax;
		return true;
	}
	return false;
}
