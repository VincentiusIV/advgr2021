#include "precomp.h" // include (only) this in every .cpp file

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	// initialize scene.
	scene = new Scene();
}

// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::Shutdown()
{
}

static Sprite rotatingGun( new Surface( "assets/aagun.tga" ), 36 );
static int frame = 0;
static float runningTime = 0;
static int fps = 0;
static std::string fpsString;

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick( float deltaTime )
{
	screen->Clear( 0 );
	
	fps = ( fps + 1000 / deltaTime ) / 2;
	fpsString = "FPS: " + std::to_string( fps );

	screen->Print( fpsString.c_str(), 2, 2, 0xffffff );

	runningTime += deltaTime / 1000;
	scene->Update();

	// draw a sprite
	rotatingGun.SetFrame( frame );
	rotatingGun.Draw( screen, 100 + 50 * sin( runningTime ), 100 + 50 * sin( PI/2 + runningTime ) );
	if ( ++frame == 36 ) frame = 0;
}
