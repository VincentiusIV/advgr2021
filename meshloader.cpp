#include "precomp.h"
#include "objloader.h"

vector<shared_ptr<MeshObject>> MeshLoader::Load( const std::string filePath)
{
	vector<shared_ptr<MeshObject>> meshes = vector<shared_ptr<MeshObject>>();
	objl::Loader loader;
	if ( loader.LoadFile( filePath ) )
	{
		for ( size_t j = 0; j < loader.LoadedMeshes.size(); j++ )
		{
			objl::Mesh mesh = loader.LoadedMeshes.at( j );
			color meshColor = color( mesh.MeshMaterial.Kd.X, mesh.MeshMaterial.Kd.Y, mesh.MeshMaterial.Kd.Z );
			shared_ptr<Material> mat = make_shared<Material>( meshColor, MaterialType::DIFFUSE );
			shared_ptr<MeshObject> newObject = make_shared<MeshObject>( mat );
			meshes.push_back( newObject );
			for ( size_t i = 0; i < mesh.Vertices.size(); i++ )
			{
				objl::Vector3 p = mesh.Vertices.at( i ).Position;
				vec3 pos = vec3( p.X, p.Y, p.Z );
				newObject->vertices.push_back( pos );
				objl::Vector3 n = mesh.Vertices.at( i ).Normal;
				newObject->normals.push_back( vec3( n.X, n.Y, n.Z ) );
				objl::Vector2 uv = mesh.Vertices.at( i ).TextureCoordinate;
				newObject->uvs.push_back( vec2( uv.X, uv.Y ) );
			}
			for ( size_t i = 0; i < mesh.Indices.size(); i++ )
			{
				uint index = mesh.Indices.at( i );
				newObject->indices.push_back( index );
			}
			newObject->triangleCount = (uint)mesh.Indices.size() / 3;
		}
	}
	std::string count = "Successfully loaded mesh with n=" + to_string( meshes.size() );
	printf( count.c_str());
	return meshes;
}
