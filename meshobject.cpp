#include "precomp.h"
#include "objloader.h"

MeshObject::MeshObject( shared_ptr<Material> material, std::string filePath ) : HittableObject( material )
{
	vertices = vector<vec3>();
	normals = vector<vec3>();
	uvs = vector<vec2>();
	indices = vector<uint>();
	triangleCount = 0;
	
	objl::Loader loader;
	if (loader.LoadFile(filePath))
	{
		for ( size_t j = 0; j < loader.LoadedMeshes.size(); j++ )
		{
			objl::Mesh mesh = loader.LoadedMeshes.at( j );
			for ( size_t i = 0; i < mesh.Vertices.size(); i++ )
			{
				objl::Vector3 p = mesh.Vertices.at( i ).Position;
				vertices.push_back( vec3( p.X, p.Y, p.Z ) );
				objl::Vector3 n = mesh.Vertices.at( i ).Normal;
				normals.push_back( vec3( n.X, n.Y, n.Z ) );
				objl::Vector2 uv = mesh.Vertices.at( i ).TextureCoordinate;
				uvs.push_back( vec2( uv.X, uv.Y ) );
			}
			for ( size_t i = 0; i < mesh.Indices.size(); i++ )
			{
				uint index = mesh.Indices.at( i );
				indices.push_back( index );
			}
			triangleCount = (uint)indices.size() / 3;
		}

	}
}

bool MeshObject::CheckRayTriangleIntersection( Ray &ray, RayHit &hit, vec3 v0, vec3 v1, vec3 v2 )
{
	vec3 v0v1 = v1 - v0;
	vec3 v0v2 = v2 - v0;
	vec3 pvec = ray.direction.cross( v0v2 );
	float determinant = v0v1.dot( pvec );
	if ( fabs( determinant ) < 0.0000001f )
		return false;
	float inverseDeterminant = 1.0 / determinant;
	vec3 tvec = ray.origin - v0;
	float u = tvec.dot( pvec ) * inverseDeterminant;
	if ( u < 0 || u > 1 )
		return false;
	vec3 qvec = tvec.cross( v0v1 );
	float v = ray.direction.dot( qvec ) * inverseDeterminant;
	if ( v < 0 || u+v > 1 )
		return false;
	float t = v0v2.dot( qvec ) * inverseDeterminant;
	if (t > 0.00001f && t < ray.t)
	{
		vec3 intersection = ray.At( t );
		if ( ( intersection - ray.origin ).sqrLentgh() > ray.tMax )
			return false;
		ray.t = t;
		hit.point = intersection;
		hit.uv.x = u;
		hit.uv.y = v;
		return true;
	}
	return false;
}

bool MeshObject::Hit( Ray &ray, RayHit &hit )
{
	uint j = 0;
	bool didHit = false;

	for ( int i = 0; i < triangleCount; i++ )
	{
		vec3 v0 = localToWorldMatrix * vertices[indices[j]];
		vec3 v1 = localToWorldMatrix * vertices[indices[j + 1]];
		vec3 v2 = localToWorldMatrix * vertices[indices[j + 2]];

		if ( CheckRayTriangleIntersection( ray, hit, v0, v1, v2) )
		{
			didHit = true;
			hit.normal = normals[indices[j]];
			hit.isFrontFace = true;
			hit.material = material;
		}

		j += 3;
	}

	return didHit;
}
