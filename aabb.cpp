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
	vec3 tsmaller = MinPerAxis( t0s, t1s );
	vec3 tbigger = MaxPerAxis( t0s, t1s );
	float tmin = fmax( -INFINITY, fmax( tsmaller.x, fmax( tsmaller.y, tsmaller.z ) ) );
	float tmax = fmin( INFINITY, fmin( tbigger.x, fmin( tbigger.y, tbigger.z ) ) );
	return ( tmin < tmax );
}
