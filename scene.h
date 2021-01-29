#pragma once

class HittableObject;
class Camera;
class Light;
class BVH;

class Scene
{
  public:
	static bool BRUTE_FORCE;
	Scene();
	~Scene();
	void Init();
	void Update(float deltaTime);
	void Add( shared_ptr<HittableObject> object );
	void Add( shared_ptr<Light> light ) { lights.push_back( light ); }
	shared_ptr<HittableObject> GetRandomEmissiveObject();
	shared_ptr<Camera> GetCamera() { return camera; };
	vector<shared_ptr<HittableObject>> objects, emissiveObjects, volumes;
	vector<shared_ptr<Light>> lights;
	BVH *bvh;

  private:	
	shared_ptr<Camera> camera;

};