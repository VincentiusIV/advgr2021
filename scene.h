#pragma once

#include "object.h"
#include "ray.h"
#include "camera.h"
#include "light.h"

class Scene
{
  public:
	Scene();
	~Scene();
	void Update(float deltaTime);
	void Add( shared_ptr<HittableObject> object );
	void Add( shared_ptr<Light> light ) { lights.push_back( light ); }
	shared_ptr<HittableObject> GetRandomEmissiveObject();
	shared_ptr<Camera> GetCamera() { return camera; };
	vector<shared_ptr<HittableObject>> objects, emissiveObjects;
	vector<shared_ptr<Light>> lights;
  private:	
	shared_ptr<Camera> camera;
	PhotonMap* photonmap;
};