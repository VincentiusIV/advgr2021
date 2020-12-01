#pragma once

struct RayHit
{
  public:
	point3 point;
	vec3 normal;
	vec2 uv;
	shared_ptr<Material> material;
	bool isFrontFace;
};

class Ray
{
  public:
	Ray() : origin( 0.0, 0.0, 0.0 ), direction( 0.0, 0.0, 0.0 ), tMax( INFINITY ), t( INFINITY ), depth(1) {}
	Ray( const point3 &origin, const vec3 &direction, float tMax, int depth)
		: origin( origin ), direction( normalize( direction ) ), tMax( tMax ), t( INFINITY ), depth(depth)
	{ }

	point3 At( float t ) { return origin + direction * t; }
	
	point3 origin;
	vec3 direction;
	float tMax, t; 
	int depth;
};