#include "precomp.h" // include (only) this in every .cpp file

static float deltaTimeInSeconds, cameraMoveSpeed = 0.2f, cameraRotateSpeed = 10.0f;
static int fps = 0;
static std::string fpsString, deltaTimeString, cameraPositionString;
static int raysPerPixel = 5000;
static float calculateChance = 0.02;
static int  frameCounter = 0, pixelCounter = 0;
static color* colorBuffer;
static int* raysCounter;

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	scene = new Scene();
	colorBuffer = (color*)MALLOC64(SCRWIDTH * SCRHEIGHT * sizeof(color));
	raysCounter = new int[SCRWIDTH * SCRHEIGHT];
	ClearColorBuffer();


	//shared_ptr<MeshObject> cube1 = make_shared<MeshObject>( normalTest, "assets/pokeball.obj" );
	//cube1->position = point3( 0, 1.5, 0.5 );
	//cube1->rotation = point3( 0, PI, 0 );
	//cube1->scale = point3( 1, 1, 1 );
	//cube1->UpdateTRS();
	//scene->Add( cube1 );

	CreateBoxEnvironment();

	//shared_ptr<DirectionalLight> sunLight = make_shared<DirectionalLight>( normalize( vec3( 0.5, -2, 1) ), 1 );
	//scene->Add( sunLight );

	//raytracer = new WhittedRayTracer(40);
	raytracer = new PathTracer(7);
}

void Tmpl8::Game::ClearColorBuffer()
{
	for ( size_t x = 0; x < SCRWIDTH; x++ )
	{
		for ( size_t y = 0; y < SCRHEIGHT; y++ )
		{
			colorBuffer[y * SCRWIDTH + x] = color( 0 );
			raysCounter[y * SCRWIDTH + x] = 1;
		}
	}
}

void Game::CreateBoxEnvironment()
{
	shared_ptr<Material> redOpaque = make_shared<Material>(color(1, 0.0, 0.0), MaterialType::DIFFUSE);
	shared_ptr<Material> normalTest = make_shared<Material>(color(0.78, 0.0, 0.0), MaterialType::NORMAL_TEST);
	shared_ptr<Material> greenMirror = make_shared<Material>(color(0.0, 0.78, 0.0), MaterialType::MIRROR);
	greenMirror->specularity = 0.9f;
	greenMirror->smoothness = 0.3;
	shared_ptr<Material> groundMirror = make_shared<Material>(color(1.0, 1.0, 1.0), MaterialType::MIRROR);
	groundMirror->specularity = 0.4f;
	groundMirror->smoothness = 1.0f;
	shared_ptr<Material> blueOpaque = make_shared<Material>(color(0.0, 0.0, 1), MaterialType::DIFFUSE);
	shared_ptr<Material> orangeOpaque = make_shared<Material>(color(1.0, 0.55, 0), MaterialType::DIFFUSE);
	shared_ptr<Material> orangeGlass = make_shared<Material>(color(1.0, 0.55, 0), MaterialType::DIELECTRIC);
	orangeGlass->n = 1.5f;
	orangeGlass->smoothness = 1.0f;
	shared_ptr<Material> beige = make_shared<Material>(color(0.9, 0.9, 0.78), MaterialType::DIFFUSE);
	shared_ptr<Material> lightMaterial = make_shared<Material>(color(0.9, 0.9, 0.9), MaterialType::EMISSIVE);

	//shared_ptr<MeshObject> cube = make_shared<MeshObject>( redOpaque, "assets/cube.obj" );
	//cube->position = point3( -2.0, 0.0, 2.0 );
	//cube->rotation = point3( 30, -30, 0 );
	//cube->scale = point3( 0.3, 0.3, 0.3 );
	//cube->UpdateTRS();
	//scene->Add( cube );


	shared_ptr<Sphere> sphere1 = make_shared<Sphere>(orangeGlass, 1);
	sphere1->position = point3(1.0, 0.0, 2.5);
	scene->Add(sphere1);

	shared_ptr<Sphere> sphere3 = make_shared<Sphere>(greenMirror, 0.7);
	sphere3->position = point3(-1.0, 0.0, 1.0);
	scene->Add(sphere3);

	shared_ptr<Sphere> lightSphere = make_shared<Sphere>( lightMaterial, 2.0 );
	lightSphere->position = point3( 0, 4.0, 4.0 );
	scene->Add( lightSphere );
	
	//shared_ptr<Sphere> lightSphere2 = make_shared<Sphere>( lightMaterial, 0.6 );
	//lightSphere2->position = point3( -2.0, 1.0, 1.0 );
	//scene->Add( lightSphere2 );

	//ground plane
	shared_ptr<Plane> plane1 = make_shared<Plane>( beige, vec3( 0, 1, 0 ), 3, 3 );
	plane1->position = point3( 0, -1, 5.0 );
	scene->Add( plane1 );

	////ceiling plane
	shared_ptr<Plane> plane5 = make_shared<Plane>( lightMaterial, vec3( 0, 1, 0 ) );
	plane5->position = point3( 0, 3.0, 5.0 );
	scene->Add( plane5 );

	////back wall plane
	shared_ptr<Plane> plane2 = make_shared<Plane>( beige, vec3( 0, 0, -1 ) );
	plane2->position = point3( -3.0, 0, 5.0 );
	scene->Add( plane2 );

	//left wall plane
	shared_ptr<Plane> plane3 = make_shared<Plane>( orangeOpaque, vec3( 1, 0, 0 ) );
	plane3->position = point3( -5.0, 0, 10.0 );
	scene->Add( plane3 );

	//right wall plane
	shared_ptr<Plane> plane4 = make_shared<Plane>( blueOpaque, vec3( 1, 0, 0 ) );
	plane4->position = point3( 3.0, 0, 5.0 );
	scene->Add( plane4 );

	//behind camera wall plane
	shared_ptr<Plane> plane6 = make_shared<Plane>( beige, vec3( 0, 0, 1 ) );
	plane6->position = point3( 0.0, 0, -5.0 );
	scene->Add( plane6 );

	shared_ptr<PointLight> sceneLight = make_shared<PointLight>( point3( 0, 1.5, 2.0 ), 5.0 );
	sceneLight->albedo = color( 0.74, 0.45, 0.22 );
	scene->Add( sceneLight );
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
	deltaTimeString = "Frame: " + std::to_string( frameCounter );
	cameraPositionString = "Camera Rotation X: " + std::to_string( scene->GetCamera()->rotation.x ) +
						   ", Y: " + std::to_string( scene->GetCamera()->rotation.y ) +
						   ", Z: " + std::to_string( scene->GetCamera()->rotation.z ) + "\n" +
						   ", Cam Fwd X: " + std::to_string( scene->GetCamera()->viewDirection.x ) +
						   ", Y: " + std::to_string( scene->GetCamera()->viewDirection.y ) +
						   ", Z: " + std::to_string( scene->GetCamera()->viewDirection.z );
	//screen->Print( fpsString.c_str(), 2, 2, 0xffffff );
	screen->Print( deltaTimeString.c_str(), 2, 10, 0xffffff );
	screen->Print( cameraPositionString.c_str(), 2, 18, 0xffffff );
}

void Game::RenderScene()
{
	++frameCounter;
	Pixel *buffer = screen->GetBuffer();
	for ( int y = 0; y < SCRHEIGHT; y++ )
	{
		for ( int x = 0; x < SCRWIDTH; x++ )
		{
			int raysForThisPixel = raysCounter[y * SCRWIDTH + x];
			if ( raysForThisPixel >= raysPerPixel || Rand( 1.0 ) > calculateChance )
				continue;
			color color = colorBuffer[y * SCRWIDTH + x];
			auto uOffset = ( 1.0 - ( 1.0 / raysForThisPixel ) ) * ( Rand( 1.0 ) ); 
			auto vOffset = ( 1.0 - ( 1.0 / raysForThisPixel ) ) * ( Rand( 1.0 ) ); 
			auto u = (( double( x ) + uOffset ) / ( SCRWIDTH - 1 ));
			auto v = 1.0 - (( double( y ) + vOffset ) / ( SCRHEIGHT - 1 ));
			Ray ray = scene->GetCamera()->CastRayFromScreenPoint( u, v );
			color += raytracer->Sample( ray, scene );
			colorBuffer[y * SCRWIDTH + x] = color;
			color = color / float( raysForThisPixel );
			buffer[y * SCRWIDTH + x] = CreateRGB( floor( color.x * 255.999 ), floor( color.y * 255.999 ), floor( color.z * 255.999 ) );
			raysCounter[y * SCRWIDTH + x] = raysForThisPixel + 1;
		}
	}

}

void Game::KeyDown( int key )
{
	KeyCode keyCode = (KeyCode)key;
	printf(to_string(key).c_str());

	switch ( keyCode )
	{
	case Tmpl8::KeyCode::W:
		scene->GetCamera()->Translate( vec3( 0, 0, cameraMoveSpeed ) * deltaTimeInSeconds );
		break;
	case Tmpl8::KeyCode::A:
		scene->GetCamera()->Translate( vec3( -cameraMoveSpeed, 0, 0 ) * deltaTimeInSeconds );
		break;
	case Tmpl8::KeyCode::S:
		scene->GetCamera()->Translate( vec3( 0, 0, -cameraMoveSpeed ) * deltaTimeInSeconds );
		break;
	case Tmpl8::KeyCode::D:
		scene->GetCamera()->Translate( vec3( cameraMoveSpeed, 0, 0 ) * deltaTimeInSeconds );
		break;
	case Tmpl8::KeyCode::R:
		scene->GetCamera()->Translate( vec3( 0, cameraMoveSpeed, 0 ) * deltaTimeInSeconds );
		break;
	case Tmpl8::KeyCode::F:
		scene->GetCamera()->Translate( vec3( 0, -cameraMoveSpeed, 0 ) * deltaTimeInSeconds );
		break;
	case KeyCode::LEFT_ARROW:
		scene->GetCamera()->Rotate( vec3( 0, -cameraRotateSpeed, 0 ) * deltaTimeInSeconds );
		break;
	case KeyCode::RIGHT_ARROW:
		scene->GetCamera()->Rotate( vec3( 0, cameraRotateSpeed, 0 ) * deltaTimeInSeconds );
		break;
	case KeyCode::UP_ARROW:
		scene->GetCamera()->Rotate( vec3( -cameraRotateSpeed, 0, 0) * deltaTimeInSeconds );
		break;
	case KeyCode::DOWN_ARROW:
		scene->GetCamera()->Rotate( vec3( cameraRotateSpeed, 0, 0 ) * deltaTimeInSeconds );
		break;
	default:
		break;
	}

	switch ( keyCode )
	{
	default:
		screen->Clear( 0 );
		ClearColorBuffer();
		break;
	}
}
