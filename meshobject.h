#pragma once

class TriangleBVH;

class MeshObject : public HittableObject
{
  public:
	static bool BRUTE_FORCE;
	MeshObject( vec3* vertices, int vertexCount, vec3* normals, vec2* uvs, uint* indices, int indexCount, shared_ptr<Material> material );
	~MeshObject();
	bool Hit( Ray &ray, RayHit &hit );
	static bool CheckRayTriangleIntersection( Ray &ray, RayHit &hit, vec3 v0, vec3 v1, vec3 v2 );
	void UpdateTRS();
	void UpdateAABB();

	vec2* uvs;
	vec3* vertices, *normals;
	uint* indices;
	vec3* worldVertices;
	int triangleCount, vertexCount, indexCount;
	TriangleBVH* subbvh;
};
