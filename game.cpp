#include "precomp.h" // include (only) this in every .cpp file

static float deltaTimeInSeconds, cameraSpeed = 0.2f;
static int fps = 0;
static std::string fpsString, deltaTimeString, cameraPositionString;
static int raysPerPixel = 1;
static float calculateChance = 0.02;
static uint originalSeed;

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	scene = new Scene();

	// initialize materials.
	shared_ptr<Material> redOpaque = make_shared<Material>(Color(0.78, 0.0, 0.0), MaterialType::DIFFUSE);
	shared_ptr<Material> greenMirror = make_shared<Material>( Color( 0.0, 0.78, 0.0 ), MaterialType::MIRROR );
	greenMirror->specularity = 0.7f;
	greenMirror->smoothness = 0.3f;
	shared_ptr<Material> redMirror = make_shared<Material>( Color( 0.78, 0.13, 0.0 ), MaterialType::MIRROR );
	redMirror->specularity = 0.7f;
	redMirror->smoothness = 1.0f;
	shared_ptr<Material> blueOpaque = make_shared<Material>( Color( 0.0, 0.0, 0.78 ), MaterialType::DIFFUSE );
	shared_ptr<Material> orangeOpaque = make_shared<Material>( Color( 1.0, 0.55, 0 ), MaterialType::DIFFUSE );
	shared_ptr<Material> orangeGlass = make_shared<Material>( Color( 1.0, 0.55, 0), MaterialType::DIELECTRIC );
	orangeGlass->n = 1.5f;
	orangeGlass->smoothness = 1.0f;
	shared_ptr<Material> beige = make_shared<Material>( Color( 0.9, 0.9, 0.78 ), MaterialType::DIFFUSE );
	
	// initialize objects
	shared_ptr<Sphere> sphere1 = make_shared<Sphere>( orangeGlass, 1 );
	sphere1->position = Point3( 1.0, 0.0, 2.5 );
	scene->Add( sphere1 );

	shared_ptr<Sphere> sphere3 = make_shared<Sphere>( greenMirror, 1 );
	sphere3->position = Point3( -1.5, 0.0, 4.0 );
	scene->Add( sphere3 );

	shared_ptr<MeshObject> cube = make_shared<MeshObject>( redOpaque, "assets/cybertruck.obj" );
	cube->position = Point3( -1.5, 0.0, 2.0 );
	scene->Add( cube );

	CreateBoxEnvironment( beige, orangeOpaque, blueOpaque );

	// initialize lights
	shared_ptr<PointLight> sceneLight = make_shared<PointLight>( Point3( 1, 0.0, 4.0 ), 2.0 );
	sceneLight->color = Color( 0.74, 0.45, 0.22 );
	scene->Add( sceneLight );

	/*shared_ptr<DirectionalLight> sunLight = make_shared<DirectionalLight>( normalize( Vector3( 0.5, -2, 1) ), 1 );
	scene->Add( sunLight );*/

	raytracer = new WhittedRayTracer(40);
	originalSeed = seed;
}

void Tmpl8::Game::CreateBoxEnvironment( std::shared_ptr<Material> &beige, std::shared_ptr<Material> &orangeOpaque, std::shared_ptr<Material> &blueOpaque )
{
	//ground plane
	shared_ptr<Plane> plane1 = make_shared<Plane>( beige, vec3( 0, 1, 0 ), 3, 3 );
	plane1->position = Point3( 0, -1, 5.0 );
	scene->Add( plane1 );

	////ceiling plane
	shared_ptr<Plane> plane5 = make_shared<Plane>( beige, vec3( 0, 1, 0 ) );
	plane5->position = Point3( 0, 5.0, 5.0 );
	scene->Add( plane5 );

	//back wall plane
	shared_ptr<Plane> plane2 = make_shared<Plane>( beige, vec3( 0, 0, -1 ) );
	plane2->position = Point3( -3.0, 0, 5.0 );
	scene->Add( plane2 );

	////left wall plane
	shared_ptr<Plane> plane3 = make_shared<Plane>( orangeOpaque, vec3( 1, 0, 0 ) );
	plane3->position = Point3( -3.0, 0, 5.0 );
	scene->Add( plane3 );

	////right wall plane
	shared_ptr<Plane> plane4 = make_shared<Plane>( blueOpaque, vec3( 1, 0, 0 ) );
	plane4->position = Point3( 3.0, 0, 5.0 );
	scene->Add( plane4 );

	//behind camera wall plane
	shared_ptr<Plane> plane6 = make_shared<Plane>( beige, vec3( 0, 0, 1 ) );
	plane6->position = Point3( 0.0, 0, -5.0 );
	scene->Add( plane6 );
}
 
// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::Shutdown()
{

}


// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick( float deltaTime )
{
	deltaTimeInSeconds = fmin(deltaTime, 1.0);
	fps = ( 1.0f / deltaTime );
	scene->Update( deltaTime );
	RenderScene();
	PrintDebugInfo( deltaTime );
}

void Game::PrintDebugInfo( float deltaTime )
{
	//fpsString = "FPS: " + std::to_string( fps );
	//deltaTimeString = "deltaTime: " + std::to_string( deltaTime );
	//cameraPositionString = "Camera X: " + std::to_string( scene->GetCamera()->position.x ) +
	//					   ", Y: " + std::to_string( scene->GetCamera()->position.y ) +
	//					   ", Z: " + std::to_string( scene->GetCamera()->position.z );
	//screen->Print( fpsString.c_str(), 2, 2, 0xffffff );
	//screen->Print( deltaTimeString.c_str(), 2, 10, 0xffffff );
	//screen->Print( cameraPositionString.c_str(), 2, 18, 0xffffff );
}

void Game::RenderScene()
{
	//screen->Clear( 0 );
	Pixel *buffer = screen->GetBuffer();
	for ( int y = 0; y < SCRHEIGHT; y++ )
	{
		for ( int x = 0; x < SCRWIDTH; x++ )
		{
			if ( buffer[y * SCRWIDTH + x] != 0 || Rand( 1.0 ) > calculateChance )
				continue;
			Color color( 0, 0, 0 );
			for ( int i = 0; i < raysPerPixel; i++ )
			{
				auto uOffset = (1.0 - ( 1.0 / raysPerPixel )) * ( Rand( 1.0 ) ); 
				auto vOffset = (1.0 - ( 1.0 / raysPerPixel )) * ( Rand( 1.0 ) ); 
				auto u = ( double( x ) + uOffset ) / ( SCRWIDTH - 1 );
				auto v = 1.0 - (( double( y ) + vOffset ) / ( SCRHEIGHT - 1 ));
				Ray ray = scene->GetCamera()->CastRayFromScreenPoint( u, v );
				color += raytracer->Trace( ray, scene );
			}
			color = color / raysPerPixel;
			buffer[y * SCRWIDTH + x] = CreateRGB( floor( color.x * 255.999 ), floor( color.y * 255.999 ), floor( color.z * 255.999 ) );
		}
	}
}

void Game::KeyDown( int key )
{
	KeyCode keyCode = (KeyCode)key;

	switch ( keyCode )
	{
	case Tmpl8::KeyCode::W:
		scene->GetCamera()->Translate( Vector3( 0, 0, cameraSpeed ) * deltaTimeInSeconds );
		break;
	case Tmpl8::KeyCode::A:
		scene->GetCamera()->Translate( Vector3( -cameraSpeed, 0, 0 ) * deltaTimeInSeconds );
		break;
	case Tmpl8::KeyCode::S:
		scene->GetCamera()->Translate( Vector3( 0, 0, -cameraSpeed ) * deltaTimeInSeconds );
		break;
	case Tmpl8::KeyCode::D:
		scene->GetCamera()->Translate( Vector3( cameraSpeed, 0, 0 ) * deltaTimeInSeconds );
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

	switch ( keyCode )
	{
	case Tmpl8::KeyCode::W:
	case Tmpl8::KeyCode::A:
	case Tmpl8::KeyCode::S:
	case Tmpl8::KeyCode::D:
	case Tmpl8::KeyCode::R:
	case Tmpl8::KeyCode::F:
		screen->Clear(0);
		break;
	default:
		break;
	}
}
