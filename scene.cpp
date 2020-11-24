#include "precomp.h"
#include "scene.h"

Scene::Scene()
{
	objects = vector<shared_ptr<HittableObject>>();
	lights = vector<shared_ptr<Light>>();
	camera = make_shared<Camera>( Point3( 0.0, 1.0, -1.0 ), Vector3( 0.0, 0.0, 1.0 ), double( SCRWIDTH ) / double(SCRHEIGHT), 1 );
}

Scene::~Scene()
{

}

void Scene::Update(float deltaTime)
{
	for ( size_t i = 0; i < objects.size(); i++ )
	{
		objects.at( i )->Update();
	}
}
