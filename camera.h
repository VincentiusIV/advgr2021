#pragma once

class Camera : public Transform
{
public:
	Camera( float aspectRatio, float focalLength ) : Camera( point3(), vec3( 0.0, 0.0, 1.0 ), aspectRatio, focalLength ) {}
	Camera( point3 position, vec3 viewDirection, float aspectRatio, float focalLength ) : Transform( position, vec3( 0.0, 0.0, 0.0 ) ), viewDirection( viewDirection ), aspectRatio( aspectRatio ), focalLength( focalLength )
	{
		RecalculateViewport();
	}

	void RecalculateViewport()
	{
		screenCenter = position + viewDirection * focalLength;
		p0 = screenCenter + vec3( -aspectRatio, -1, 0 );
		p1 = screenCenter + vec3( aspectRatio, -1, 0 );
		p2 = screenCenter + vec3( -aspectRatio, 1, 0 );
	}

	void Translate( vec3 delta ) 
	{ 
		position += delta; 
		RecalculateViewport();
	}

	point3 PointOnScreeen(float u, float v)
	{
		return p0 + u * ( p1 - p0 ) + v * ( p2 - p0 );
	}

	Ray CastRayFromScreenPoint( float u, float v )
	{
		vec3 dir = normalize( PointOnScreeen( u, v ) - position );
		return Ray( position, dir, INFINITY, 0);
	}

	vec3 Forward() { return normalize(vec3(cos(rotation.y)*cos(rotation.x), sin(rotation.y)*cos(rotation.x), sin(rotation.x))); }
	vec3 viewDirection;
	float aspectRatio, focalLength;
	point3 screenCenter, p0, p1, p2;
};