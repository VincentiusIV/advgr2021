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

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick( float deltaTime )
{
	screen->Clear( 0 );
	screen->Print( "hello world", 2, 2, 0xffffff );

	runningTime += deltaTime / 100;
	scene->Update();

	// draw a sprite
	rotatingGun.SetFrame( frame );
	rotatingGun.Draw( screen, 100 + 50 * sin( runningTime ), 100 + 50 * sin( PI/2 + runningTime ) );
	if ( ++frame == 36 ) frame = 0;
}
