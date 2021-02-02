#include "precomp.h" // include (only) this in every .cpp file
#include <omp.h>

static float deltaTimeInSeconds, cameraMoveSpeed = 0.2f, cameraRotateSpeed = 10.0f;
static int fps = 0;
static std::string fpsString, deltaTimeString, cameraPositionString;
static int raysPerPixel = 5000, rowCount = 32;
// random chance that a pixel is calculated during a frame, reduces time to get something on the screen. 1.0 = always.
static float calculateChance = 1; 
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
	scene->GetCamera()->Translate( vec3( 0, 2.5, -2) );
	scene->GetCamera()->Rotate( vec3( 0.0, 0.0, 0.0 ) );

	Scene::BRUTE_FORCE = false;
	MeshObject::BRUTE_FORCE = false;

	colorBuffer = (color*)MALLOC64(SCRWIDTH * SCRHEIGHT * sizeof(color));
	raysCounter = new int[SCRWIDTH * SCRHEIGHT];
	ClearColorBuffer();
	CreateBoxEnvironment();

	//raytracer = new WhittedRayTracer(scene, 7);
	raytracer = new VolumetricPathTracer( scene, 24 );
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
	shared_ptr<Material> white = make_shared<Material>(color(0.95, 0.95, 0.95), MaterialType::DIFFUSE);
	shared_ptr<Material> checkerboard = make_shared<Material>(color(0.7, 0.7, 0.7), MaterialType::DIFFUSE);
	checkerboard->isCheckerboard = true;
	shared_ptr<Material> lightMaterial = make_shared<Material>(color(0.9, 1.0, 1.0), MaterialType::EMISSIVE);
	textureDiffuse->mainTex = new Surface( "assets/apartment/building_col_3.jpg" );
	shared_ptr<Material> fog = make_shared<Material>( color( 0.9, 1.0, 1.0 ), MaterialType::VOLUMETRIC );

	// Floors & Walls
	shared_ptr<Plane> groundFloor = make_shared<Plane>( checkerboard, vec3( 0, 1, 0 ), 3, 3 );
	groundFloor->position = point3( 0, 0, 0.0 );
	groundFloor->scale = point3( 1000, 1, 1000 );
	scene->Add( groundFloor );

	//shared_ptr<Plane> leftWall = make_shared<Plane>( blueOpaque, vec3( 0.5, 0.5, 0), 3, 3 );
	//leftWall->position = point3( 3, 3, 3 );
	//leftWall->scale = point3( 2, 2, 2 );
	//scene->Add( leftWall );

	//shared_ptr<Plane> leftWall = make_shared<Plane>( blueOpaque, vec3( 1, 0, 0 ), 3, 3 );
	//leftWall->position = point3( 1, 4, 3 );
	//leftWall->scale = point3( 2, 4, 2 );
	//scene->Add( leftWall );
	//shared_ptr<Plane> rightWall = make_shared<Plane>( redOpaque, vec3( -1, 0, 0 ), 3, 3 );
	//rightWall->position = point3( -1, 4, 3 );
	//rightWall->scale = point3( 2, 4, 2 );
	//scene->Add( rightWall );
	shared_ptr<Plane> ceiling = make_shared<Plane>( white, vec3( 0, 0, 1 ), 3, 3 );
	ceiling->position = point3( 0, 3, 2 );
	ceiling->scale = point3( 3, 10, 1 );
	//scene->Add( ceiling );
	//shared_ptr<Plane> bottom = make_shared<Plane>( white, vec3( 0, -1, 0 ), 3, 3 );
	//bottom->position = point3( 0, 2, 3 );
	//bottom->scale = point3( 2, 2, 2 );
	//scene->Add( bottom );
	//shared_ptr<Plane> frontWall = make_shared<Plane>( white, vec3( 0, 0, 1 ), 3, 3 );
	//frontWall->position = point3( 0, 4, 4 );
	//frontWall->scale = point3( 2, 4, 2 );
	//scene->Add( frontWall );


	//shared_ptr<Plane> slid1 = make_shared<Plane>( white, vec3( 0, 0, 1 ), 3, 3 );
	//slid1->position = point3( 0, 2.5, 2 );
	//slid1->scale = point3( 2, 0.5, 2 );
	//scene->Add( slid1 );

	//shared_ptr<Plane> slid2 = make_shared<Plane>( white, vec3( 0, 0, 1 ), 3, 3 );
	//slid2->position = point3( 0, 3.25, 2 );
	//slid2->scale = point3( 2, 0.5, 2 );
	//scene->Add( slid2 );

	//shared_ptr<Plane> slid3 = make_shared<Plane>( white, vec3( 0, 0, 1 ), 3, 3 );
	//slid3->position = point3( 0, 4, 2 );
	//slid3->scale = point3( 2, 0.5, 2 );
	//scene->Add( slid3 );

	//shared_ptr<Plane> slid4 = make_shared<Plane>( white, vec3( 0, 0, 1 ), 3, 3 );
	//slid4->position = point3( 0, 4.75, 2 );
	//slid4->scale = point3( 2, 0.5, 2 );
	//scene->Add( slid4 );

	//shared_ptr<Plane> slid5 = make_shared<Plane>( white, vec3( 0, 0, 1 ), 3, 3 );
	//slid5->position = point3( 0, 5.5, 2 );
	//slid5->scale = point3( 2, 0.5, 2 );
	//scene->Add( slid5 );

	//shared_ptr<Plane> backWall2 = make_shared<Plane>( white, vec3( 0, 0, 1 ), 3, 3 );
	//backWall2->position = point3( 0, 4, 8 );
	//backWall2->scale = point3( 100, 100, 100 );
	//scene->Add( backWall2 );

	//// Spheres
	shared_ptr<Sphere> baseSphere = make_shared<Sphere>( textureDiffuse, 1.0 );
	baseSphere->position = point3( 2.5, 1.0, 2 );
	scene->Add( baseSphere );
	//shared_ptr<Sphere> baseSphere2 = make_shared<Sphere>( groundMirror, 2.0 );
	//baseSphere2->position = point3( -6, 2, 6 );
	//scene->Add( baseSphere2 );

	//shared_ptr<Sphere> baseSphere1 = make_shared<Sphere>( glass, 1 );
	//baseSphere1->position = point3( 1.5, -1.5, 1.5 );
	//scene->Add( baseSphere1 );

	//shared_ptr<Sphere> baseSphere2 = make_shared<Sphere>( beige, 2 );
	//baseSphere2->position = point3( -2.5, -1.5, 2.5 );
	//scene->Add( baseSphere2 );

	shared_ptr<Sphere> sphere1 = make_shared<Sphere>(glass, 1);
	sphere1->position = point3( -2.5, 1.0, 2 );
	//scene->Add(sphere1);

	//shared_ptr<Sphere> sphere2 = make_shared<Sphere>( groundMirror, 1 );
	//sphere2->position = point3( 2, 1, 6);
	//scene->Add( sphere2 );

//	vector<shared_ptr<MeshObject>> meshObject1 = MeshLoader::Load( "assets/alien/Alien Animal_front_redux.obj" );
//
//#pragma omp parallel for schedule( dynamic, 1 )
//	for ( int i = 0; i < meshObject1.size(); i++ )
//	{
//		shared_ptr<MeshObject> current = meshObject1.at( i );
//		current->position = point3( 0, 0, 9 );
//		current->rotation = vec3( 0, 180, 0 );
//		current->scale = point3( 0.32 );
//		current->UpdateTRS();
//		if ( !MeshObject::BRUTE_FORCE )
//			current->subbvh->ConstructBVH();
//	}
//	for ( size_t i = 0; i < meshObject1.size(); i++ )
//	{
//		shared_ptr<MeshObject> current = meshObject1.at( i );
//		scene->Add( current );
//	}

	//shared_ptr<Sphere> sphere4 = make_shared<Sphere>( textureDiffuse, 1 );
	//sphere4->position = point3( -2.5, 1.5, 2.5 );
	//scene->Add( sphere4 );

	//shared_ptr<Sphere> lightSphere2 = make_shared<Sphere>( lightMaterial, 1 );
	//lightSphere2->position = point3( 5, 4, -5 );
	//scene->Add( lightSphere2 );

	shared_ptr<Sphere> lightSphere = make_shared<Sphere>( lightMaterial, 0.6 );
	lightSphere->position = point3( 0, 2, 5 );
	scene->Add( lightSphere );

	// TODO: Fix Plane Area Lights
	//shared_ptr<Plane> ceilingLight = make_shared<Plane>( lightMaterial, vec3( 0, -1, 0 ), 1, 1 );
	//ceilingLight->position = point3( 0, 2.45, 0 );
	//ceilingLight->scale = point3( 2, 1, 2 );
	//scene->Add( ceilingLight );

	//// Volume
	shared_ptr<Sphere> volume = make_shared<Sphere>( fog, 100 );
	volume->position = point3( 0, 0, 0 );
	scene->Add( volume );

	for ( size_t i = 0; i < scene->objects.size(); i++ )
	{
		shared_ptr<HittableObject> obj = scene->objects.at( i );
		obj->UpdateAABB();
	}

	scene->Init();

	printf( "* * * Initialization finished * * *" );
	
}

void Tmpl8::Game::CreateMeshEnvironment()
{
	shared_ptr<Material> checkerboard = make_shared<Material>( color( 0.7, 0.7, 0.7 ), MaterialType::DIFFUSE );
	checkerboard->isCheckerboard = true;
	shared_ptr<Material> lightMaterial = make_shared<Material>( color( 0.9, 1.0, 1.0 ), MaterialType::EMISSIVE );

	shared_ptr<Sphere> lightSphere = make_shared<Sphere>( lightMaterial, 3 );
	lightSphere->position = point3( 6, 12, 6 );
	scene->Add( lightSphere );
	//ground plane
	shared_ptr<Plane> plane1 = make_shared<Plane>( checkerboard, vec3( 0, 1, 0 ), 3, 3 );
	plane1->position = point3( 0, 0, 0.0 );
	plane1->scale = point3( 100, 1, 100 );
	scene->Add( plane1 );

	vector<shared_ptr<MeshObject>> meshObject1 = MeshLoader::Load( "assets/alien/Alien Animal.obj" );

	#pragma omp parallel for schedule( dynamic, 1 )
	for ( int i = 0; i < meshObject1.size(); i++ )
	{
		shared_ptr<MeshObject> current = meshObject1.at( i );
		current->scale = point3( 0.12 );
		current->UpdateTRS();
		if(!MeshObject::BRUTE_FORCE)
			current->subbvh->ConstructBVH();
	}
	for ( size_t i = 0; i < meshObject1.size(); i++ )
	{
		shared_ptr<MeshObject> current = meshObject1.at( i );
		scene->Add( current );
	}
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
