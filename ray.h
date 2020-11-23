#pragma once

struct RayHit
{
  public:
	Point3 point;
	Vector3 normal;
	shared_ptr<Material> material;
	bool isFrontFace;
};

class Ray
{
  public:
	Ray() : origin( 0.0, 0.0, 0.0 ), direction( 0.0, 0.0, 0.0 ), tMax( INFINITY ), t( INFINITY ), depth(1) {}
	Ray( const Point3 &origin, const Vector3 &direction, float tMax, int depth)
		: origin( origin ), direction( normalize( direction ) ), tMax( tMax ), t( INFINITY ), depth(depth)
	{ }

	Point3 At( float t ) { return origin + direction * t; }
	
	Point3 origin;
	Vector3 direction;
	float tMax, t; 
	int depth;
};


class RayDifferential : public Ray
{
	// contains additional information about two auxiliary rays for better antialiasing.
};