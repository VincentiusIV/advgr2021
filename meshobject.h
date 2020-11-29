#pragma once

class MeshObject : public HittableObject
{
  public:
	MeshObject( shared_ptr<Material> material, std::string filePath ) ;
	bool Hit( Ray &ray, RayHit &hit );
	bool CheckRayTriangleIntersection( Ray &ray, RayHit &hit, vec3 v0, vec3 v1, vec3 v2 );
	void UpdateTRS();
	vector<vec2> uvs;
	vector<vec3> vertices, normals;
	vector<uint> indices;
	vector<vec3> worldVertices;
	int triangleCount;
	float r2;
};
