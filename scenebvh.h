#pragma once

class SceneBVH : public BVH
{
  public:
	SceneBVH( Scene *scene ) : scene( scene ){}
	void ConstructBVH();
	AABB CalculateBounds( int first, int count );
	bool IntersectNode( BVHNode &node, Ray &r, RayHit &hit );
	vec3 GetPosition( int objIdx ) { return scene->objects.at(objIdx)->position; }

	Scene *scene;
};
