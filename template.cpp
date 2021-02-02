// Template, UU version
// IGAD/NHTV/UU - Jacco Bikker - 2006-2019

// Note:
// this version of the template uses SDL2 for all frame buffer interaction
// see: https://www.libsdl.org

#ifdef _MSC_VER
#pragma warning (disable : 4530) // complaint about exception handler
#pragma warning (disable : 4273)
#pragma warning (disable : 4311) // pointer truncation from HANDLE to long
#endif

#include "precomp.h"

namespace Tmpl8 {

// Math Stuff
// ----------------------------------------------------------------------------
vec3 normalize( const vec3& v ) { return v.normalized(); }
vec3 cross( const vec3& a, const vec3& b ) { return a.cross( b ); }
float dot( const vec3& a, const vec3& b ) { return a.dot( b ); }
vec3 lerp( const vec3 &a, const vec3 &b, const float &t ) { return ((1.0-t) * a + t*b); }
vec3 reflect( const vec3 &dir, const vec3 &normal ) { return ( dir - 2.0f * dir.dot( normal ) * normal ); }
vec3 refract( const vec3 &incomingDir, const vec3 &normal, const float &reflectionRatio )
{
	float cosTheta = fmax( -1.0, fmin( 1.0f, incomingDir.dot( normal ) ) );
	float etai = 1.0f, etat = reflectionRatio;
	vec3 n = normal;
	if ( cosTheta < 0 )
	{
		cosTheta = -cosTheta;
	}
	else
	{
		std::swap( etai, etat );
		n = -normal;
	}
	float etaiOverEtat = etai / etat;
	float k = 1.0f - etaiOverEtat * etaiOverEtat * ( 1.0f - cosTheta * cosTheta );
	return k < 0 ? 0.0f : etaiOverEtat * incomingDir + ( etaiOverEtat * cosTheta - sqrtf( k ) ) * n;
}
vec3 project( const vec3 &dir, const vec3 &n )
{
	vec3 planeNormal = normalize( n );
	return dir - ( dir.dot( planeNormal ) / (planeNormal.length() * planeNormal.length()) ) * planeNormal;
}
vec3 operator * ( const float& s, const vec3& v ) { return vec3( v.x * s, v.y * s, v.z * s ); }
vec3 operator * ( const vec3& v, const float& s ) { return vec3( v.x * s, v.y * s, v.z * s ); }
vec4 operator * ( const float& s, const vec4& v ) { return vec4( v.x * s, v.y * s, v.z * s, v.w * s ); }
vec4 operator * ( const vec4& v, const float& s ) { return vec4( v.x * s, v.y * s, v.z * s, v.w * s ); }
mat4 operator * ( const mat4& a, const mat4& b )
{
	mat4 r;
	for (uint i = 0; i < 16; i += 4) for (uint j = 0; j < 4; ++j)
		r[i + j] = (b.cell[i + 0] * a.cell[j + 0]) + (b.cell[i + 1] * a.cell[j + 4]) + (b.cell[i + 2] * a.cell[j + 8]) + (b.cell[i + 3] * a.cell[j + 12]);
	return r;
}
bool operator == ( const mat4& a, const mat4& b ) { for (uint i = 0; i < 16; i++) if (a.cell[i] != b.cell[i]) return false; return true; }
bool operator != ( const mat4& a, const mat4& b ) { return !(a == b); }
vec4 operator * ( const mat4& a, const vec4& b )
{
	return vec4( a.cell[0] * b.x + a.cell[1] * b.y + a.cell[2] * b.z + a.cell[3] * b.w,
		a.cell[4] * b.x + a.cell[5] * b.y + a.cell[6] * b.z + a.cell[7] * b.w,
		a.cell[8] * b.x + a.cell[9] * b.y + a.cell[10] * b.z + a.cell[11] * b.w,
		a.cell[12] * b.x + a.cell[13] * b.y + a.cell[14] * b.z + a.cell[15] * b.w );
}

vec4 operator * ( const vec4& b, const mat4& a )
{
	return vec4( a.cell[0] * b.x + a.cell[1] * b.y + a.cell[2] * b.z + a.cell[3] * b.w,
		a.cell[4] * b.x + a.cell[5] * b.y + a.cell[6] * b.z + a.cell[7] * b.w,
		a.cell[8] * b.x + a.cell[9] * b.y + a.cell[10] * b.z + a.cell[11] * b.w,
		a.cell[12] * b.x + a.cell[13] * b.y + a.cell[14] * b.z + a.cell[15] * b.w );
}

vec3 operator*( const mat4 &a, const vec3 &b )
{
	vec4 r = a * vec4( b, 1.0 );
	return vec3(r.x, r.y, r.z);
}

vec3 operator*( const vec3 &a, const mat4 &b )
{
	vec4 r = b * vec4( a, 1.0 );
	return vec3( r.x, r.y, r.z );
}

vec3 RandomInsideUnitSphere()
{
	return normalize(vec3( Rand( 2.0f ) - 1.0f, Rand( 2.0f ) - 1.0f, Rand( 2.0f ) - 1.0f )); 
}

const vec3 MinPerAxis( const vec3 &a, const vec3 &b )
{
	return vec3( min( a.x, b.x ), min( a.y, b.y ), min( a.z, b.z ) );
}

const vec3 MaxPerAxis( const vec3 &a, const vec3 &b )
{
	return vec3( max( a.x, b.x ), max( a.y, b.y ), max( a.z, b.z ) );
}

mat4 mat4::rotate( const vec3 l, const float a )
{
	// http://inside.mines.edu/fs_home/gmurray/ArbitraryAxisRotation
	mat4 M;
	const float u = l.x, v = l.y, w = l.z, ca = cosf( a ), sa = sinf( a );
	M.cell[0] = u * u + (v * v + w * w) * ca, M.cell[1] = u * v * (1 - ca) - w * sa;
	M.cell[2] = u * w * (1 - ca) + v * sa, M.cell[4] = u * v * (1 - ca) + w * sa;
	M.cell[5] = v * v + (u * u + w * w) * ca, M.cell[6] = v * w * (1 - ca) - u * sa;
	M.cell[8] = u * w * (1 - ca) - v * sa, M.cell[9] = v * w * (1 - ca) + u * sa;
	M.cell[10] = w * w  + (u * u + v * v) * ca;
	M.cell[3] = M.cell[7] = M.cell[11] = M.cell[12] = M.cell[13] = M.cell[14] = 0, M.cell[15] = 1;
	return M;
}

mat4 mat4::rotatex( const float a )
{
	mat4 M;
	const float ca = cosf( a ), sa = sinf( a );
	M.cell[5] = ca, M.cell[6] = -sa;
	M.cell[9] = sa, M.cell[10] = ca;
	return M;
}
mat4 mat4::rotatey( const float a )
{
	mat4 M;
	const float ca = cosf( a ), sa = sinf( a );
	M.cell[0] = ca, M.cell[2] = sa;
	M.cell[8] = -sa, M.cell[10] = ca;
	return M;
}
mat4 mat4::rotatez( const float a )
{
	mat4 M;
	const float ca = cosf( a ), sa = sinf( a );
	M.cell[0] = ca, M.cell[1] = -sa;
	M.cell[4] = sa, M.cell[5] = ca;
	return M;
}

mat4 mat4::rotateDegrees( const vec3 rotation )
{
	vec3 rad;
	rad.x = rotation.x * DEG_TO_RAD;
	rad.y = rotation.y * DEG_TO_RAD;
	rad.z = rotation.z * DEG_TO_RAD;
	return rotatex( rad.x ) * rotatey( rad.y ) * rotatez( rad.z );
}

mat4 mat4::translate( const vec3 translation )
{
	mat4 M = mat4::identity();
	M.cell[3] = translation.x;
	M.cell[7] = translation.y;
	M.cell[11] = translation.z;
	return M;
}

mat4 mat4::scale( const vec3 scale )
{
	mat4 M = mat4::identity();
	M.cell[0] = scale.x;
	M.cell[5] = scale.y;
	M.cell[10] = scale.z;
	return M;
}

mat4 mat4::trs( const vec3 translation, const vec3 rotation, const vec3 scale )
{
	return mat4::scale( scale ) * mat4::rotateDegrees( rotation ) * mat4::translate( translation );
}

void NotifyUser( const char *s )
{
#ifdef _WIN32
	HWND hApp = FindWindow( NULL, TEMPLATE_VERSION );
	MessageBox( hApp, s, "ERROR", MB_OK );
#else
	std::cout << "ERROR: " << s << std::endl;
#endif
	exit( 0 );
}
}

using namespace Tmpl8;
using namespace std;

#ifdef ADVANCEDGL

PFNGLGENBUFFERSPROC glGenBuffers = 0;
PFNGLBINDBUFFERPROC glBindBuffer = 0;
PFNGLBUFFERDATAPROC glBufferData = 0;
PFNGLMAPBUFFERPROC glMapBuffer = 0;
PFNGLUNMAPBUFFERPROC glUnmapBuffer = 0;
typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
unsigned int framebufferTexID[2];
GLuint fbPBO[2];
unsigned char* framedata = 0;

#endif

int ACTWIDTH, ACTHEIGHT;
static bool firstframe = true;

Surface* surface = 0;
Game* game = 0;
SDL_Window* window = 0;

#ifdef _MSC_VER
void redirectIO()
{
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	AllocConsole();
	GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), &coninfo );
	coninfo.dwSize.Y = 500;
	SetConsoleScreenBufferSize( GetStdHandle( STD_OUTPUT_HANDLE ), coninfo.dwSize );
	HANDLE h1 = GetStdHandle( STD_OUTPUT_HANDLE );
	int h2 = _open_osfhandle( (intptr_t)h1, _O_TEXT );
	FILE* fp = _fdopen( h2, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );
	h1 = GetStdHandle( STD_INPUT_HANDLE ), h2 = _open_osfhandle( (intptr_t)h1, _O_TEXT );
	fp = _fdopen( h2, "r" ), *stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );
	h1 = GetStdHandle( STD_ERROR_HANDLE ), h2 = _open_osfhandle( (intptr_t)h1, _O_TEXT );
	fp = _fdopen( h2, "w" ), *stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
	ios::sync_with_stdio();
	freopen( "CON", "w", stdout );
	freopen( "CON", "w", stderr );
}
#endif

#ifdef ADVANCEDGL

bool createFBtexture()
{
	glGenTextures( 2, framebufferTexID );
	if (glGetError()) return false;
	for ( int i = 0; i < 2; i++ )
	{
		glBindTexture( GL_TEXTURE_2D, framebufferTexID[i] );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, SCRWIDTH, SCRHEIGHT, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL );
		glBindTexture(GL_TEXTURE_2D, 0);
		if (glGetError()) return false;
	}
	const int sizeMemory = 4 * SCRWIDTH * SCRHEIGHT;
	glGenBuffers( 2, fbPBO );
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, fbPBO[0] );
	glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, sizeMemory, NULL, GL_STREAM_DRAW_ARB );
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, fbPBO[1] );
	glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, sizeMemory, NULL, GL_STREAM_DRAW_ARB );
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
	glBindTexture( GL_TEXTURE_2D, framebufferTexID[0] );
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, fbPBO[0] );
	framedata = (unsigned char*)glMapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB );
	if (!framedata) return false;
	memset( framedata, 0, SCRWIDTH * SCRHEIGHT * 4 );
	return (glGetError() == 0);
}

bool init()
{
	fbPBO[0] = fbPBO[1] = -1;
	glGenBuffers =  (PFNGLGENBUFFERSPROC)wglGetProcAddress( "glGenBuffersARB" );
	glBindBuffer =  (PFNGLBINDBUFFERPROC)wglGetProcAddress( "glBindBufferARB" );
	glBufferData =  (PFNGLBUFFERDATAPROC)wglGetProcAddress( "glBufferDataARB" );
	glMapBuffer  =  (PFNGLMAPBUFFERPROC)wglGetProcAddress( "glMapBufferARB" );
	glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress( "glUnmapBufferARB" );
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
	if ((!glGenBuffers) || (!glBindBuffer) || (!glBufferData) || (!glMapBuffer) || (!glUnmapBuffer)) return false;
	if (glGetError()) return false;
	glViewport( 0, 0, SCRWIDTH, SCRHEIGHT );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, 1, 0, 1, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glEnable( GL_TEXTURE_2D );
	glShadeModel( GL_SMOOTH );
	if (!createFBtexture()) return false;
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	if (wglSwapIntervalEXT) wglSwapIntervalEXT( 0 );
	surface = new Surface( SCRWIDTH, SCRHEIGHT, 0, SCRWIDTH );
	return true;
}

void swap()
{
	static int index = 0;
	int nextindex;
	glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB );
	glBindTexture( GL_TEXTURE_2D, framebufferTexID[index] );
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, fbPBO[index] );
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, SCRWIDTH, SCRHEIGHT, GL_BGRA, GL_UNSIGNED_BYTE, 0 );
	nextindex = (index + 1) % 2;
	index = (index + 1) % 2;
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, fbPBO[nextindex] );
	framedata = (unsigned char*)glMapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB );
	glColor3f( 1.0f, 1.0f, 1.0f );
	glBegin( GL_QUADS );
	glNormal3f( 0, 0, 1 );
	glTexCoord2f( 0.0f, 0.0f );
	glVertex2f  ( 0.0f, 1.0f );
	glTexCoord2f( 1.0f, 0.0f );
	glVertex2f  ( 1.0f, 1.0f );
	glTexCoord2f( 1.0f, 1.0f );
	glVertex2f  ( 1.0f, 0.0f );
	glTexCoord2f( 0.0f, 1.0f );
	glVertex2f  ( 0.0f, 0.0f );
	glEnd();
	glBindTexture( GL_TEXTURE_2D, 0 );
	SDL_GL_SwapWindow( window );
}

#endif

int main( int argc, char **argv )
{
#ifdef _MSC_VER
	redirectIO();
#endif
	printf( "application started.\n" );
	SDL_Init( SDL_INIT_VIDEO );
#ifdef ADVANCEDGL
#ifdef FULLSCREEN
	window = SDL_CreateWindow( TEMPLATE_VERSION, 100, 100, SCRWIDTH, SCRHEIGHT, SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL );
#else
	window = SDL_CreateWindow( TEMPLATE_VERSION, 100, 100, SCRWIDTH, SCRHEIGHT, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL );
#endif
	SDL_GLContext glContext = SDL_GL_CreateContext( window);
	init();
	ShowCursor( false );
#else
#ifdef FULLSCREEN
	window = SDL_CreateWindow( TEMPLATE_VERSION, 100, 100, SCRWIDTH, SCRHEIGHT, SDL_WINDOW_FULLSCREEN );
#else
	window = SDL_CreateWindow( TEMPLATE_VERSION, 100, 100, SCRWIDTH, SCRHEIGHT, SDL_WINDOW_SHOWN );
#endif
	surface = new Surface( SCRWIDTH, SCRHEIGHT );
	surface->Clear( 0 );
	SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
	SDL_Texture* frameBuffer = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCRWIDTH, SCRHEIGHT );
#endif
	int exitapp = 0;
	game = new Game();
	game->SetTarget( surface );
	timer t;
	t.reset();
	while (!exitapp)
	{
	#ifdef ADVANCEDGL
		swap();
		surface->SetBuffer( (Pixel*)framedata );
	#else
		void* target = 0;
		int pitch;
		SDL_LockTexture( frameBuffer, NULL, &target, &pitch );
		if (pitch == (surface->GetWidth() * 4))
		{
			memcpy( target, surface->GetBuffer(), SCRWIDTH * SCRHEIGHT * 4 );
		}
		else
		{
			unsigned char* t = (unsigned char*)target;
			for( int i = 0; i < SCRHEIGHT; i++ )
			{
				memcpy( t, surface->GetBuffer() + i * SCRWIDTH, SCRWIDTH * 4 );
				t += pitch;
			}
		}
		SDL_UnlockTexture( frameBuffer );
		SDL_RenderCopy( renderer, frameBuffer, NULL, NULL );
		SDL_RenderPresent( renderer );
	#endif
		if (firstframe)
		{
			game->Init();
			firstframe = false;
		}
		// calculate frame time and pass it to game->Tick
		game->Tick( t.elapsed() );
		t.reset();
		// event loop
		SDL_Event event;
		while (SDL_PollEvent( &event ))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				exitapp = 1;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					exitapp = 1;
					// find other keys here: http://sdl.beuc.net/sdl.wiki/SDLKey
				}
				game->KeyDown( event.key.keysym.scancode );
				break;
			case SDL_KEYUP:
				game->KeyUp( event.key.keysym.scancode );
				break;
			case SDL_MOUSEMOTION:
				game->MouseMove( event.motion.x, event.motion.y );
				break;
			case SDL_MOUSEBUTTONUP:
				game->MouseUp( event.button.button );
				break;
			case SDL_MOUSEBUTTONDOWN:
				game->MouseDown( event.button.button );
				break;
			default:
				break;
			}
		}
	}
	game->Shutdown();
	SDL_Quit();
	return 1;
}
