#include "precomp.h" // include (only) this in every .cpp file

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	// initialize scene.
	shared_ptr<Material> redOpaque = make_shared<Material>(Color(200.000, 0.0, 0.0), MaterialType::DIFFUSE);
	shared_ptr<Sphere> sphere1 = make_shared<Sphere>(redOpaque, 1);
	sphere1->position = Point3(0.0, 0.0, -5.0);
	
	scene = new Scene();
	scene->Add( sphere1 );

	raytracer = new WhittedRayTracer();
}

// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::Shutdown()
{

}

static float runningTime = 0;
static int fps = 0;
static std::string fpsString;

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick( float deltaTime )
{
	fps = ( 1000 / deltaTime );
	fpsString = "FPS: " + std::to_string( fps );
	runningTime += deltaTime / 1000;

	screen->Clear( 0 );
	scene->Update(deltaTime);

	RenderScene();

	screen->Print( fpsString.c_str(), 2, 2, 0xffffff );
}

void Game::RenderScene()
{
	Pixel *buffer = screen->GetBuffer();
	for ( int y = 0; y < SCRHEIGHT; y++ )
	{
		for ( int x = 0; x < SCRWIDTH; x++ )
		{
			auto u = double( x ) / ( SCRWIDTH - 1 );
			auto v = double( y ) / ( SCRHEIGHT - 1 );
			Ray ray = scene->GetCamera()->CastRayFromScreenPoint( u, v );
			Color color = raytracer->Trace( ray, scene );
			buffer[y * SCRWIDTH + x] = CreateRGB( color.x, color.y, color.z );
		}
	}
}
