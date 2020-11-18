#include "precomp.h"

bool NearestIntersection( Scene *scene, Ray ray, RayHit& hit)
{
	for ( size_t i = 0; i < scene->objects.size(); i++ )
	{
		shared_ptr<HittableObject> obj = scene->objects.at( i );
		if (obj->Hit(ray, hit))
		{
			return true;
		}
	}
	return false;
}

float DirectIllumination(Point3 point, Vector3 normal)
{
	return 1.0f;
}

Color WhittedRayTracer::Trace( Ray ray, Scene *scene )
{
	RayHit hit;
	if (NearestIntersection(scene, ray, hit))
	{
		switch (hit.material->materialType)
		{
			case MaterialType::DIFFUSE:
				return hit.material->color * DirectIllumination( hit.point, hit.normal );
			default:
				return Color(0.0, 0.0, 0.0);
		}
	}
	else
	{
		vec3 unit_direction = ray.direction;
		auto t = 0.5 * ( -unit_direction.y + 1.0 );
		Color color = ( 1.0 - t ) * Color( 255.999, 255.999, 255.999 ) + t * Color( 127.999, 179.222, 255.999 );
		return color;
	}
}
