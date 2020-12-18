#pragma once

class Ray;

struct AABB
{
  public:
	AABB() : min(), max() {}
	AABB( const vec3 &min, const vec3 &max ) : min( min ), max( max ) {}
	bool Contains( const point3 &p );
	bool Intersect( Ray &r );
	vec3 min, max;

	float Area() { return ( max.x - min.x ) * ( max.y - min.y ) * ( max.z - min.z ); }
};
