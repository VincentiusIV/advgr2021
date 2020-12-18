#include "precomp.h" // include (only) this in every .cpp file
#include <omp.h>

static float deltaTimeInSeconds, cameraMoveSpeed = 0.2f, cameraRotateSpeed = 10.0f;
static int fps = 0;
static std::string fpsString, deltaTimeString, cameraPositionString;
static int raysPerPixel = 5000, rowCount = 32;
// random chance that a pixel is calculated during a frame, reduces time to get something on the screen. 1.0 = always.
static float calculateChance = 0.05; 
static int  frameCounter = 0, pixelCounter = 0;
static color* colorBuffer;
static int* raysCounter;
static bool visualizeBvh = false;
static int maxBvhDepth = 100;
// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	scene = new Scene();
	scene->GetCamera()->Translate( vec3( 1.5, 1.0, 3 ) );
	scene->GetCamera()->Rotate( vec3( 1.0, -120.0, 0.0 ) );

	colorBuffer = (color*)MALLOC64(SCRWIDTH * SCRHEIGHT * sizeof(color));
	raysCounter = new int[SCRWIDTH * SCRHEIGHT];
	ClearColorBuffer();
	CreateBoxEnvironment();

	Scene::BRUTE_FORCE = false;
	MeshObject::BRUTE_FORCE = false;

	//raytracer = new WhittedRayTracer(scene, 7);
	raytracer = new PathTracer( scene, 7 );
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
	shared_ptr<Material> redOpaque = make_shared<Material>(color(0.9, 0.1, 0.1), MaterialType::DIFFUSE);
	shared_ptr<Material> cyanOpaque = make_shared<Material>(color(0.0, 1, 1), MaterialType::DIFFUSE);
	shared_ptr<Material> textureDiffuse = make_shared<Material>(color(0.9, 0.1, 0.1), MaterialType::DIFFUSE);
	textureDiffuse->mainTex = new Surface( "assets/marble.PNG" );
	shared_ptr<Material> normalTest = make_shared<Material>( color( 0.78, 0.1, 0.1 ), MaterialType::NORMAL_TEST );
	shared_ptr<Material> uvTest = make_shared<Material>( color( 0.78, 0.1, 0.1 ), MaterialType::UV_TEST );
	shared_ptr<Material> greenMirror = make_shared<Material>( color( 0.1, 0.9, 0.1 ), MaterialType::MIRROR );
	greenMirror->specularity = 0.9f;
	greenMirror->smoothness = 0.9f;
	shared_ptr<Material> groundMirror = make_shared<Material>(color(1.0, 1.0, 1.0), MaterialType::MIRROR);
	groundMirror->specularity = 0.9f;
	groundMirror->smoothness = 0.9f;
	shared_ptr<Material> blueOpaque = make_shared<Material>( color( 0.1, 0.1, 1 ), MaterialType::DIFFUSE );
	shared_ptr<Material> orangeOpaque = make_shared<Material>( color( 1.0, 0.55, 0.1 ), MaterialType::DIFFUSE );
	shared_ptr<Material> glass = make_shared<Material>( color( 1.0, 0.55, 0.1 ), MaterialType::DIELECTRIC );
	glass->n = 1.5f;
	glass->smoothness = 1.0f;
	shared_ptr<Material> beige = make_shared<Material>(color(0.7, 0.7, 0.7), MaterialType::DIFFUSE);
	shared_ptr<Material> checkerboard = make_shared<Material>(color(0.7, 0.7, 0.7), MaterialType::DIFFUSE);
	checkerboard->isCheckerboard = true;
	shared_ptr<Material> lightMaterial = make_shared<Material>(color(0.9, 1.0, 1.0), MaterialType::EMISSIVE);

	shared_ptr<Sphere> lightSphere = make_shared<Sphere>( lightMaterial, 3 );
	lightSphere->position = point3( 6, 12, 6 );
	scene->Add( lightSphere );
	//ground plane
	shared_ptr<Plane> plane1 = make_shared<Plane>( checkerboard, vec3( 0, 1, 0 ), 3, 3 );
	plane1->position = point3( 0, 0, 0.0 );
	plane1->scale = point3( 100, 1, 100 );
	scene->Add( plane1 );

	//shared_ptr<Plane> plane2 = make_shared<Plane>( beige, vec3( 1, 0, 0 ), 3, 3 );
	//plane2->position = point3( 5, 5, 5 );
	//plane2->scale = point3( 1, 1, 1 );
	//scene->Add( plane2 );

	//shared_ptr<Plane> plane3 = make_shared<Plane>( redOpaque, vec3( -1, 0, 0 ), 3, 3 );
	//plane3->position = point3( 7, 5, 5 );
	//plane3->scale = point3( 10, 10, 10 );
	//scene->Add( plane3 );

	vector<shared_ptr<MeshObject>> cybertruck = MeshLoader::Load( "assets/apartment/Futuristic_Apartment.obj" );

	#pragma omp parallel for schedule( dynamic, 1 )
	for ( int i = 0; i < cybertruck.size(); i++ )
	{
		shared_ptr<MeshObject> current = cybertruck.at( i );
		current->scale = point3( 0.1 );
		current->UpdateTRS();
		current->subbvh->ConstructBVH();
	}

	for ( size_t i = 0; i < cybertruck.size(); i++ )
	{
		shared_ptr<MeshObject> current = cybertruck.at( i );
		scene->Add( current );
	}

	//for ( int x = -5; x < 5; x++ )
	//{
	//	for ( int z = -5; z < 5; z++ )
	//	{
	//		shared_ptr<Sphere> baseSphere = make_shared<Sphere>( beige, 0.5 );
	//		baseSphere->position = point3( x, 0.0, z);
	//		scene->Add( baseSphere );
	//	}
	//}

	//shared_ptr<Sphere> baseSphere = make_shared<Sphere>( beige, 2 );
	//baseSphere->position = point3( 2.5, -1.5, 2.5 );
	//scene->Add( baseSphere );
	//shared_ptr<Sphere> baseSphere1 = make_shared<Sphere>( beige, 2 );
	//baseSphere1->position = point3( 0, -1.5, 2.5 );
	//scene->Add( baseSphere1 );
	//shared_ptr<Sphere> baseSphere2 = make_shared<Sphere>( beige, 2 );
	//baseSphere2->position = point3( -2.5, -1.5, 2.5 );
	//scene->Add( baseSphere2 );

	//shared_ptr<Sphere> sphere1 = make_shared<Sphere>(glass, 1);
	//sphere1->position = point3(0, 1.5, 2.5);
	//scene->Add(sphere1);

	//shared_ptr<Sphere> sphere2 = make_shared<Sphere>( groundMirror, 1 );
	//sphere2->position = point3( 2.5, 1.5, 2.5 );
	//scene->Add( sphere2 );

	//shared_ptr<Sphere> sphere4 = make_shared<Sphere>( textureDiffuse, 1 );
	//sphere4->position = point3( -2.5, 1.5, 2.5 );
	//scene->Add( sphere4 );

	//shared_ptr<Sphere> lightSphere2 = make_shared<Sphere>( lightMaterial, 1 );
	//lightSphere2->position = point3( 5, 4, -5 );
	//scene->Add( lightSphere2 );

	shared_ptr<PointLight> sceneLight = make_shared<PointLight>( point3( 0, 4.0, 2.0 ), 10.0 );
	sceneLight->albedo = color( 0.74, 0.45, 0.22 );
	scene->Add( sceneLight );

	shared_ptr<PointLight> sceneLight2 = make_shared<PointLight>( point3( -1, 2.5, 0 ), 5.0 );
	sceneLight2->albedo = color( 0.74, 0.45, 0.22 );
	scene->Add( sceneLight2 );

	for ( size_t i = 0; i < scene->objects.size(); i++ )
	{
		shared_ptr<HittableObject> obj = scene->objects.at( i );
		obj->UpdateAABB();
	}

	scene->Init();

}
 
// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::Shutdown()
{
	delete scene;
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

	#pragma omp parallel for schedule( dynamic, 1 )
	for ( int i = 0; i < rowCount; i++ )
	{
		int beginIdx = ( double(i) / double(rowCount) ) * SCRHEIGHT;
		int endIdx = ( double( i + 1 ) / double( rowCount ) ) * SCRHEIGHT;
		for ( int y = beginIdx; y < endIdx; y++ )
		{
			for ( int x = 0; x < SCRWIDTH; x++ )
			{
				int raysForThisPixel = raysCounter[y * SCRWIDTH + x];
				if ( raysForThisPixel >= raysPerPixel || Rand( 1.0 ) > calculateChance )
					continue;
				color pixelColor = colorBuffer[y * SCRWIDTH + x];
				auto uOffset = ( Rand( 2.0 ) - 1.0 );
				auto vOffset = ( Rand( 2.0 ) - 1.0 );
				auto u = ( ( double( x ) + uOffset ) / ( SCRWIDTH - 1 ) );
				auto v = 1.0 - ( ( double( y ) + vOffset ) / ( SCRHEIGHT - 1 ) );
				Ray ray = scene->GetCamera()->CastRayFromScreenPoint( u, v );
				pixelColor += raytracer->Sample( ray );

				colorBuffer[y * SCRWIDTH + x] = pixelColor;
				pixelColor = pixelColor / float( raysForThisPixel );
				pixelColor.x = clamp( pixelColor.x, 0.0f, 1.0f );
				pixelColor.y = clamp( pixelColor.y, 0.0f, 1.0f );
				pixelColor.z = clamp( pixelColor.z, 0.0f, 1.0f );
				if ( visualizeBvh )
				{
					maxBvhDepth = max( ray.bvhDepth, maxBvhDepth );
					pixelColor += color( 0.9, 0.1, 0.1 ) * ( double( ray.bvhDepth ) / double( maxBvhDepth ) ) + color( 0.1, 0.9, 0.1 ) * ( 1.0 - ( double( ray.bvhDepth ) / double( maxBvhDepth ) ) );
					pixelColor *= 0.5f;
				}
				buffer[y * SCRWIDTH + x] = CreateRGB( floor( pixelColor.x * 255.999 ), floor( pixelColor.y * 255.999 ), floor( pixelColor.z * 255.999 ) );
				raysCounter[y * SCRWIDTH + x] = raysForThisPixel + 1;
			}
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
	case KeyCode::F1:
		visualizeBvh = !visualizeBvh;
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
