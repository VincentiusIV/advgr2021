#pragma once

class Camera : public Transform
{
public:
	Camera( float aspectRatio, float focalLength ) : Camera( point3(), vec3( 0.0, 0.0, 1.0 ), aspectRatio, focalLength ) {}
	Camera( point3 position, vec3 rotation, float aspectRatio, float focalLength ) : Transform( position, rotation ), aspectRatio( aspectRatio ), focalLength( focalLength ), viewDirection( 0, 0, 1 )
	{
		RecalculateViewport();
	}

	void RecalculateViewport()
	{
		viewDirection = CalculateForwardDirection(); //vec3(0,0,1) * mat4::rotateDegrees(rotation);
		viewDirection = normalize( viewDirection );
		screenCenter = position + viewDirection * focalLength;
		p0 = screenCenter + vec3( -aspectRatio, -1, 0 )*mat4::rotateDegrees(rotation);
		p1 = screenCenter + vec3( aspectRatio, -1, 0 ) * mat4::rotateDegrees( rotation );
		p2 = screenCenter + vec3( -aspectRatio, 1, 0 ) * mat4::rotateDegrees( rotation );
	}

	void Translate( vec3 delta ) 
	{ 
		position += delta * mat4::rotateDegrees( rotation ); 
		RecalculateViewport();
	}

	void Rotate(vec3 delta)
	{
		rotation += delta;
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

	vec3 CalculateForwardDirection() { 
		vec3 rad;
		rad.x = rotation.x * DEG_TO_RAD;
		rad.y = rotation.y * DEG_TO_RAD;
		rad.z = rotation.z * DEG_TO_RAD;
		vec3 dir;
		dir.x = -cos( rad.x ) * sin( rad.y );
		dir.y = sin( rad.x );
		dir.z = -cos( rad.x ) * cos( rad.y );
		return dir; 
	}
	vec3 viewDirection;
	float aspectRatio, focalLength;
	point3 screenCenter, p0, p1, p2;
};