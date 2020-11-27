#pragma once

class MeshObject : public HittableObject
{
  public:
	MeshObject( shared_ptr<Material> material, std::string filePath ) ;
	bool Hit( Ray &ray, RayHit &hit );

	vector<vec2> uvs;
	vector<vec3> vertices, normals;
	vector<uint> vertexIndices, uvIndices, normalIndices;
};
