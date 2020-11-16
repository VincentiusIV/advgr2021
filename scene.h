#pragma once

#include "object.h"

class Scene
{
  public:
	Scene();
	~Scene();
	void Update();
	void Add( shared_ptr<HittableObject> object );

  private:	
	vector<shared_ptr<HittableObject>> objects;
};