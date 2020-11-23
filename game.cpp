#include "precomp.h" // include (only) this in every .cpp file

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	scene = new Scene();

	// initialize materials.
	shared_ptr<Material> redOpaque = make_shared<Material>(Color(200.000, 0.0, 0.0), MaterialType::DIFFUSE);
	shared_ptr<Material> greenOpaque = make_shared<Material>( Color( 0.0, 200.000, 0.0 ), MaterialType::DIFFUSE );
	shared_ptr<Material> blueOpaque = make_shared<Material>( Color( 0.0, 0.0, 200.000 ), MaterialType::NORMAL_TEST );
	shared_ptr<Material> beige = make_shared<Material>( Color( 245.000, 245.000, 220.000 ), MaterialType::DIFFUSE );
	

	// initialize objects
	shared_ptr<Sphere> sphere1 = make_shared<Sphere>(redOpaque, 1);
	sphere1->position = Point3( 0.0, 0.0, 5.0 );
	scene->Add( sphere1 );

	shared_ptr<Sphere> sphere3 = make_shared<Sphere>( greenOpaque, 1 );
	sphere3->position = Point3( 3.0, 0.0, 5.0 );
	scene->Add( sphere3 );

	shared_ptr<Sphere> sphere2 = make_shared<Sphere>( blueOpaque, 1 );
	sphere2->position = Point3( -3.0, 0.0, 5.0 );
	scene->Add( sphere2 );

	//shared_ptr<Sphere> groundSphere = make_shared<Sphere>( beige, 10000 );
	//groundSphere->position = Point3( 0.0, -10001, 5.0 );
	//scene->Add( groundSphere );

	shared_ptr<Plane> plane1 = make_shared<Plane>( beige, vec3( 0, 1, 0 ) );
	plane1->position = Point3( 0, -10, 6.0 );
	scene->Add( plane1 );

	shared_ptr<Plane> plane2 = make_shared<Plane>( redOpaque, vec3( 1, 0, 0 ) );
	plane2->position = Point3( -12.0, 0, 6.0 );
	scene->Add( plane2 );

	// initialize lights
	shared_ptr<Light> sunLight = make_shared<Light>( Point3( 5.0, 3.0, 0.0 ), 1 );
	scene->Add( sunLight );

	shared_ptr<Light> sunLight1 = make_shared<Light>( Point3( -5.0, 3.0, 0.0 ), 1 );
	scene->Add( sunLight1 );
	
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
			auto v = 1.0 - double( y ) / ( SCRHEIGHT - 1 );
			Ray ray = scene->GetCamera()->CastRayFromScreenPoint( u, v );
			Color color = raytracer->Trace( ray, scene );
			buffer[y * SCRWIDTH + x] = CreateRGB( color.x, color.y, color.z );
		}
	}
}
