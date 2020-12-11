#pragma once

class BVHNode
{
  public:
	BVHNode();
	void Subdivide();

	aabb bounds;
	bool isLeaf;
	shared_ptr<BVHNode> left, right;
	int first, count;
};