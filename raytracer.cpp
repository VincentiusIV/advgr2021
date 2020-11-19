#include "precomp.h"

bool NearestIntersection( Scene *scene, Ray ray, RayHit& hit)
{
	bool hitAny = false;
	for ( size_t i = 0; i < scene->objects.size(); i++ )
	{
		shared_ptr<HittableObject> obj = scene->objects.at( i );
		hitAny |= ( obj->Hit( ray, hit ) );
	}
	return hitAny;
}

float DirectIllumination(Scene* scene, Point3 point, Vector3 normal)
{
	float illumination = 0.0f;
	for ( size_t i = 0; i < scene->lights.size(); i++ )
	{
		shared_ptr<Light> light = scene->lights.at( i );
		Ray shadowRay = Ray(point, normalize(light->position - point), INFINITY);
		RayHit hit;
		if (NearestIntersection(scene, shadowRay, hit))
		{
			continue;
		}
		else
		{
			float d = dot(normalize( normal), normalize(shadowRay.direction ));
			if (d >= 0)
				illumination += d;
		}
	}

	return illumination;
}

Color WhittedRayTracer::Trace( Ray ray, Scene *scene )
{
	RayHit hit;
	if (NearestIntersection(scene, ray, hit))
	{
		switch (hit.material->materialType)
		{
			case MaterialType::DIFFUSE:
				return hit.material->color * DirectIllumination( scene, hit.point, hit.normal );
			case MaterialType::NORMAL_TEST:
				return 128.000 * Color( hit.normal.x + 1, hit.normal.y + 1, hit.normal.z + 1 ) * DirectIllumination(scene, hit.point, hit.normal);
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
