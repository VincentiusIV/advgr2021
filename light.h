#pragma once

#include "transform.h"

class Light : public Transform
{
  public:
	Light( Point3 position, float intensity ) : Transform(position), intensity( intensity ), color(1.0, 1.0, 1.0) {}
	
	virtual float Illuminate( const Point3 &point, const Vector3 &normal, const Ray &shadowRay ) = 0;
	virtual Ray CastShadowRayFrom( const Point3 &point )
	{
		return Ray( point, normalize( this->position - point ), ( this->position - point ).sqrLentgh(), 0 );
	}

	float intensity;
	Color color;
};

class PointLight : public Light
{
  public:
	PointLight( Point3 position, float intensity, float range ) : Light( position, intensity ), range(range) {}

	float Illuminate( const Point3 &point, const Vector3 &normal, const Ray &shadowRay )
	{
		float dist = ( point - this->position ).length();
		return dot( normalize( normal ), shadowRay.direction ) * (1 - (dist/range)) * intensity;
	}

	float range;
};

class DirectionalLight : public Light
{
  public:
	DirectionalLight( Vector3 direction, float intensity ) : Light( Point3( 0.0, 0.0, 0.0 ), intensity ), direction(normalize( direction )) 
	{

	}

	Ray CastShadowRayFrom( const Point3 &point )
	{
		return Ray( point, -direction, INFINITY, 0 );
	}

	float Illuminate( const Point3 &point, const Vector3 &normal, const Ray &shadowRay )
	{
		return dot( normalize( normal ), -direction ) * intensity;
	}

	Vector3 direction;
};