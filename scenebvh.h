#pragma once

class SceneBVH : public BVH
{
  public:
	SceneBVH( Scene *scene, int N ) : BVH( N ), scene( scene )
	{

	}
	AABB CalculateBounds( int first, int count );
	bool IntersectNode( BVHNode &node, Ray &r, RayHit &hit );
	vec3 GetObjectPosition( uint objIdx ) { return scene->objects.at( objIdx )->position; }
	AABB GetObjectAABB( uint objIdx ) { return scene->objects.at( objIdx )->aabb; }
	Scene *scene;
};
