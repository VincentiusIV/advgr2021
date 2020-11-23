#include "precomp.h" // include (only) this in every .cpp file

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	scene = new Scene();

	// initialize materials.
	shared_ptr<Material> redOpaque = make_shared<Material>(Color(0.78, 0.0, 0.0), MaterialType::DIFFUSE);
	shared_ptr<Material> greenMirror = make_shared<Material>( Color( 0.0, 0.78, 0.0 ), MaterialType::MIRROR );
	shared_ptr<Material> blueOpaque = make_shared<Material>( Color( 0.0, 0.0, 0.78 ), MaterialType::NORMAL_TEST );
	shared_ptr<Material> orangeGlass = make_shared<Material>( Color( 0.0, 0.0, 0.78 ), MaterialType::DIELECTRIC );
	orangeGlass->n = 1.3f;
	shared_ptr<Material> beige = make_shared<Material>( Color( 0.9, 0.9, 0.78 ), MaterialType::DIFFUSE );
	

	// initialize objects
	//shared_ptr<Sphere> sphere1 = make_shared<Sphere>(orangeGlass, 1);
	//sphere1->position = Point3( 2.0, 0.0, 3.0 );
	//scene->Add( sphere1 );

	shared_ptr<Sphere> sphere3 = make_shared<Sphere>( greenMirror, 1 );
	sphere3->position = Point3( -3.0, 0.0, 5.0 );
	scene->Add( sphere3 );

	shared_ptr<Sphere> sphere2 = make_shared<Sphere>( redOpaque, 0.5 );
	sphere2->position = Point3( -2.0, 0.0, 5.0 );
	scene->Add( sphere2 );

	//shared_ptr<Sphere> groundSphere = make_shared<Sphere>( beige, 10000 );
	//groundSphere->position = Point3( 0.0, -10001, 5.0 );
	//scene->Add( groundSphere );

	//shared_ptr<Plane> plane1 = make_shared<Plane>( blueOpaque, vec3( 0, 1, 0 ) );
	//plane1->position = Point3( 0, -1, 0 );
	//scene->Add( plane1 );

	shared_ptr<Plane> plane2 = make_shared<Plane>( blueOpaque, vec3( 1, 0, 0 ) );
	plane2->position = Point3( -7.0, 0, 0 );
	scene->Add( plane2 );

	//shared_ptr<Plane> plane3 = make_shared<Plane>( blueOpaque, vec3( -1, 0, 0 ) );
	//plane3->position = Point3( 7.0, 0, 0 );
	//scene->Add( plane3 );

	// initialize lights
	shared_ptr<Light> sunLight = make_shared<Light>( Point3( 0.0, 3.0, 0.0 ), 1 );
	scene->Add( sunLight );
	
	raytracer = new WhittedRayTracer(10);
}
 
// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::Shutdown()
{

}

static float runningTime = 0, deltaTimeInSeconds;
static int fps = 0;
static std::string fpsString;
static float cameraSpeed = 30.0f;
static int maxPixelsPerFrame = 30;
static int lastX = 0, lastY = 0;

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick( float deltaTime )
{
	deltaTimeInSeconds = deltaTime/1000.0f;
	fps = ( 1.0f / deltaTime );
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
			
			buffer[y * SCRWIDTH + x] = CreateRGB( (int)floor(color.x * 255.999), (int)floor(color.y * 255.999), (int)floor(color.z * 255.999 ));
		}
	}
}

void Game::KeyDown( int key )
{
	KeyCode keyCode = (KeyCode)key;

	switch ( keyCode )
	{
	case Tmpl8::KeyCode::W:
		scene->GetCamera()->Translate( Vector3( 0, 0, -cameraSpeed ) * deltaTimeInSeconds );
		break;
	case Tmpl8::KeyCode::A:
		scene->GetCamera()->Translate( Vector3( cameraSpeed, 0, 1 ) * deltaTimeInSeconds );
		break;
	case Tmpl8::KeyCode::S:
		scene->GetCamera()->Translate( Vector3( 0, 0, cameraSpeed ) * deltaTimeInSeconds );
		break;
	case Tmpl8::KeyCode::D:
		scene->GetCamera()->Translate( Vector3( -cameraSpeed, 0, 0 ) * deltaTimeInSeconds );
		break;
	case Tmpl8::KeyCode::R:
		scene->GetCamera()->Translate( Vector3( 0, cameraSpeed, 0 ) * deltaTimeInSeconds );
		break;
	case Tmpl8::KeyCode::F:
		scene->GetCamera()->Translate( Vector3( 0, -cameraSpeed, 0 ) * deltaTimeInSeconds );
		break;
	default:
		break;
	}
}
