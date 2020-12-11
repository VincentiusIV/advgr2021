#pragma once

class BVH
{
  public:
	BVH();
	
	void ConstructBVH( Scene *scene );
	bool Intersect( Ray &r, RayHit &hit );
	void FindSplitPlane();

	shared_ptr<BVHNode> root;
};
