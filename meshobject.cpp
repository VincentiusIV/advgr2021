#include "precomp.h"

bool MeshObject::BRUTE_FORCE = false;

MeshObject::MeshObject( shared_ptr<Material> material ) : HittableObject( material )
{
	vertices = vector<vec3>();
	normals = vector<vec3>();
	uvs = vector<vec2>();
	indices = vector<uint>();
	triangleCount = 0;	
	subbvh = new TriangleBVH( this );
	subbvh->maxObjectsPerLeaf = 3;
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
	if (t > 0.00001f && t < ray.t && t <= ray.tMax)
	{
		ray.t = t;
		hit.point = ray.At( t );
		hit.normal = normalize(v0v1.cross( v0v2 ));
		if ( hit.normal.dot( ray.direction ) > 0.0000001f )
			hit.normal = -hit.normal;
		hit.uv.x = u;
		hit.uv.y = v;
		return true;
	}
	return false;
}

void MeshObject::UpdateTRS()
{
	localToWorldMatrix = mat4::trs( position, rotation, scale );
	worldVertices.clear();
	for ( size_t i = 0; i < vertices.size(); i++ )
	{
		worldVertices.push_back( localToWorldMatrix * vertices.at( i ) );
	}

	if(!MeshObject::BRUTE_FORCE)
		subbvh->ConstructBVH();
}

void MeshObject::UpdateAABB()
{
	vec3 bmin = vec3( 3.40282e+038 ), bmax = vec3( 1.17549e-038 );
	for ( size_t i = 0; i < worldVertices.size(); i++ )
	{
		vec3 p = worldVertices.at( i );
		bmin = MinPerAxis( bmin, p );
		bmax = MaxPerAxis( bmax, p );
	}
	aabb = AABB( bmin, bmax );
}

bool MeshObject::Hit( Ray &ray, RayHit &hit )
{
	if ( !aabb.Intersect( ray ) )
		return false;

	if (MeshObject::BRUTE_FORCE)
	{
		uint j = 0;
		bool didHit = false;
		for ( int i = 0; i < triangleCount; i++ )
		{
			vec3 v0 = worldVertices[indices[j]];
			vec3 v1 = worldVertices[indices[j + 1]];
			vec3 v2 = worldVertices[indices[j + 2]];
			if ( CheckRayTriangleIntersection( ray, hit, v0, v1, v2) )
			{
				didHit = true;
				//hit.normal = normals[indices[j]];
				hit.isFrontFace = true;
				hit.material = material;
			}
			j += 3;
		}
		return didHit;
	}
	else
	{
		if ( subbvh->Intersect( ray, hit ) )
		{
			hit.isFrontFace = true;
			hit.material = material;
			return true;
		}
		return false;
	}
}

