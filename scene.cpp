#include "precomp.h"

bool Scene::BRUTE_FORCE = false;

Scene::Scene()
{
	objects = vector<shared_ptr<HittableObject>>();
	volumes = vector<shared_ptr<HittableObject>>();
	lights = vector<shared_ptr<Light>>();
	camera = make_shared<Camera>( point3( 0.0), vec3( 0.0 ), double( SCRWIDTH ) / double(SCRHEIGHT), 1 );
}

void Scene::Init()
{
	if (!BRUTE_FORCE)
	{
		bvh = new SceneBVH( this, objects.size() );
		bvh->ConstructBVH();
	}

	//photonmap = new PhotonMap(this);
	
}


Scene::~Scene()
{

}

void Scene::Add(shared_ptr<HittableObject> object)
{
	if ( object->material->materialType == MaterialType::VOLUMETRIC )
	{
		volumes.push_back( object );
		return;
	}
	objects.push_back( object ); 
	if ( object->material->materialType == MaterialType::EMISSIVE )
		emissiveObjects.push_back( object );
}

void Scene::Update(float deltaTime)
{
	for ( size_t i = 0; i < objects.size(); i++ )
	{
		objects.at( i )->Update();
	}
}

shared_ptr<HittableObject> Scene::GetRandomEmissiveObject()
{
	int randIdx = RandomFloat() * emissiveObjects.size();
	if ( randIdx >= emissiveObjects.size() )
		randIdx = emissiveObjects.size() - 1;
	return emissiveObjects.at( randIdx );
}

