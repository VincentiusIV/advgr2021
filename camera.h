#pragma once

class Camera : public Transform
{
public:
	Camera( float aspectRatio, float focalLength ) : Camera( Point3(), Vector3( 0.0, 0.0, 1.0 ), aspectRatio, focalLength ) {}
	Camera( Point3 position, Vector3 viewDirection, float aspectRatio, float focalLength ) : Transform( position, Vector3( 0.0, 0.0, 0.0 ) ), aspectRatio( aspectRatio ), focalLength( focalLength )
	{
		screenCenter = position + viewDirection * focalLength;
		p0 = screenCenter + Vector3( -aspectRatio, -1, 0 );
		p1 = screenCenter + Vector3( aspectRatio, -1, 0 );
		p2 = screenCenter + Vector3( -aspectRatio, 1, 0 );
	}
	~Camera() {}

	Point3 PointOnScreeen(float u, float v)
	{
		return p0 + u * ( p1 - p0 ) + v * ( p2 - p0 );
	}

	Ray CastRayFromScreenPoint( float u, float v )
	{
		Vector3 dir = normalize( PointOnScreeen( u, v ) - position );
		return Ray( position, dir, INFINITY );
	}

	Vector3 Forward() { return normalize(Vector3(cos(rotation.y)*cos(rotation.x), sin(rotation.y)*cos(rotation.x), sin(rotation.x))); }
	float aspectRatio, focalLength;
	Point3 screenCenter, p0, p1, p2;
};