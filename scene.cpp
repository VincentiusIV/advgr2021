#include "precomp.h"

Scene::Scene()
{
	objects = vector<shared_ptr<HittableObject>>();
	lights = vector<shared_ptr<Light>>();
	camera = make_shared<Camera>( point3( 0.0, 1.0, -2.5 ), vec3( 0.0, 0.0, 0.0 ), double( SCRWIDTH ) / double(SCRHEIGHT), 1 );
}

Scene::~Scene()
{

}

void Scene::Add(shared_ptr<HittableObject> object)
{
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
