#pragma once

struct RayHit
{
  public:
	Point3 point;
	Vector3 normal;
	shared_ptr<Material> material;
};

class Ray
{
  public:
	Ray() : origin(0.0, 0.0, 0.0), direction(0.0, 0.0, 0.0), tMax(INFINITY), time(0) {}
	Ray( const Point3 &origin, const Vector3 &direction, float tMax, float time)
		: origin( origin ), direction( direction ), tMax( tMax ), time(time)
	{ }

	Point3 At( float t ) { return origin + direction * t; }
	
	Point3 origin;
	Vector3 direction;
	mutable float tMax; // allows us to restrict the ray to a segment of points (73)
	float time; 
};

class RayDifferential : public Ray
{
	// contains additional information about two auxiliary rays for better antialiasing.
};