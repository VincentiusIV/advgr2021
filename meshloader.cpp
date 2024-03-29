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
			objl::Mesh &mesh = loader.LoadedMeshes.at( j );
			
			color meshColor = color( mesh.MeshMaterial.Kd.X, mesh.MeshMaterial.Kd.Y, mesh.MeshMaterial.Kd.Z );
			shared_ptr<Material> mat = make_shared<Material>( meshColor, MaterialType::DIFFUSE );
			mat->mainTex = new Surface(( "assets/" + mesh.MeshMaterial.map_Kd ).c_str());

			int vertexCount = mesh.Vertices.size();
			int indexCount = mesh.Indices.size();
			shared_ptr<MeshObject> newObject = make_shared<MeshObject>( vertexCount, indexCount, mat );

			for ( size_t i = 0; i < vertexCount; i++ )
			{
				objl::Vector3 p = mesh.Vertices.at( i ).Position;
				vec3 pos = vec3( p.X, p.Y, p.Z );
				newObject->vertices[i] = ( pos );
				objl::Vector3 n = mesh.Vertices.at( i ).Normal;
				newObject->normals[i] = ( vec3( n.X, n.Y, n.Z ) );
				objl::Vector2 uv = mesh.Vertices.at( i ).TextureCoordinate;
				newObject->uvs[i] = vec2( uv.X, uv.Y );
			}
			for ( size_t i = 0; i < indexCount; i++ )
			{
				int index = mesh.Indices.at( i );
				if (index < 0 || index >= vertexCount)
				{
					printf( "wtf" );
				}
				newObject->indices[i] = index;
			}

			meshes.push_back( newObject );
		}
	}
	std::string count = "Successfully loaded mesh with n=" + to_string( meshes.size() );
	printf( count.c_str());
	return meshes;
}
