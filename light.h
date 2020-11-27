#pragma once

#include "transform.h"


class Light : public Transform
{
  public:
	Light( point3 position, float intensity ) : Transform(position), intensity( intensity ), albedo(1.0, 1.0, 1.0) {}
	
	virtual color Illuminate( const point3 &point, const vec3 &normal, const Ray &shadowRay ) = 0;
	virtual Ray CastShadowRayFrom( const point3 &point )
	{
		return Ray( point, normalize( this->position - point ), ( this->position - point ).sqrLentgh(), 0 );
	}

	float intensity;
	color albedo;
};

class PointLight : public Light
{
  public:
	PointLight( point3 position, float intensity ) : Light( position, intensity ) {}

	color Illuminate( const point3 &point, const vec3 &normal, const Ray &shadowRay )
	{
		float dist = ( point - this->position ).sqrLentgh();
		return albedo * dot( normal, shadowRay.direction ) * ( intensity / dist);
	}
};

class DirectionalLight : public Light
{
  public:
	DirectionalLight( vec3 direction, float intensity ) : Light( point3( 0.0, 0.0, 0.0 ), intensity ), direction(normalize( direction )) 
	{

	}

	Ray CastShadowRayFrom( const point3 &point )
	{
		return Ray( point, -direction, INFINITY, 0 );
	}

	color Illuminate( const point3 &point, const vec3 &normal, const Ray &shadowRay )
	{
		return albedo * (dot( normalize( normal ), -direction ) * intensity);
	}

	vec3 direction;
};