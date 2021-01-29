#pragma once

class HittableObject;

struct RayHit
{
  public:
	point3 point;
	vec3 normal;
	vec2 uv;
	shared_ptr<Material> material;
	shared_ptr<HittableObject> hitObject;
	bool isFrontFace;
	float tNear, tFar;
};

class Ray
{
  public:
	Ray() : origin( 0.0, 0.0, 0.0 ), direction( 0.0, 0.0, 0.0 ), tMax( INFINITY ), t( INFINITY ), depth( 1 ), bvhDepth( 0 ) {}
	Ray( const Ray &ray ) : Ray( ray.origin, ray.direction, ray.tMax, ray.depth ) {}
	Ray( const point3 &origin, const vec3 &direction, float tMax, int depth)
		: origin( origin ), direction( normalize( direction ) ), tMax( tMax ), t( INFINITY ), depth( depth ), bvhDepth(0)
	{ }

	point3 At( float t ) { return origin + direction * t; }
	
	point3 origin;
	vec3 direction;
	float tMax, t; 
	int depth, bvhDepth;
};