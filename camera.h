#pragma once

class Camera : public Transform
{
public:
	Camera(float fov, float aspectRatio);
	~Camera();
	float fov, aspectRatio;
};