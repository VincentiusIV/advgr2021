#pragma once

class Camera : public Transform
{
public:
	Camera();
	~Camera();
	float fov, aspectRatio;
};