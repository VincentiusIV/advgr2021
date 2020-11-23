#pragma once

namespace Tmpl8 {

enum class KeyCode
{
	W = 119,
	A = 97,
	S = 115,
	D = 100,
	R = 114,
	F = 102
};

class Game
{
public:
	void SetTarget( Surface* surface ) { screen = surface; }
	void Init();
	void Shutdown();
	void Tick( float deltaTime );
	void RenderScene();
	void MouseUp( int button ) { /* implement if you want to detect mouse button presses */ }
	void MouseDown( int button ) { /* implement if you want to detect mouse button presses */ }
	void MouseMove( int x, int y ) { /* implement if you want to detect mouse movement */ }
	void KeyUp( int key ) { /* implement if you want to handle keys */ }
	void KeyDown( int key );

  private:
	Surface* screen;
	Scene *scene;
	RayTracer *raytracer;
};

}; // namespace Tmpl8