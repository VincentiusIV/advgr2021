#pragma once

class TriangleBVH;

class MeshObject : public HittableObject
{
  public:
	static bool BRUTE_FORCE;
	MeshObject( shared_ptr<Material> material );
	bool Hit( Ray &ray, RayHit &hit );
	static bool CheckRayTriangleIntersection( Ray &ray, RayHit &hit, vec3 v0, vec3 v1, vec3 v2 );
	void UpdateTRS();
	void UpdateAABB();
	vector<vec2> uvs;
	vector<vec3> vertices, normals;
	vector<uint> indices;
	vector<vec3> worldVertices;
	int triangleCount;
	TriangleBVH* subbvh;
};
