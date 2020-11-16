#include "precomp.h"
#include "scene.h"

Scene::Scene()
{
	objects = vector<shared_ptr<HittableObject>>();

	// Define your scene

}

Scene::~Scene()
{

}

void Scene::Update()
{
	printf( "Updating scene.\n" );
	for ( size_t i = 0; i < objects.size(); i++ )
	{
		objects.at( i )->Update();
	}
}

void Scene::Add( shared_ptr<HittableObject> object )
{
	objects.push_back( object );
}
