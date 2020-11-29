#pragma once

namespace Tmpl8 {

enum class KeyCode
{
	W = 26,
	A = 4,
	S = 22,
	D = 7,
	R = 21,
	F = 9,
	LEFT_ARROW = 80,
	RIGHT_ARROW = 79,
	UP_ARROW = 82,
	DOWN_ARROW = 81
};

class Game
{
public:
	void SetTarget( Surface* surface ) { screen = surface; }
  void Init();
  void CreateBoxEnvironment( std::shared_ptr<Material> &beige, std::shared_ptr<Material> &orangeOpaque, std::shared_ptr<Material> &blueOpaque );
	void Shutdown();
	void Tick( float deltaTime );
	void PrintDebugInfo( float deltaTime );
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