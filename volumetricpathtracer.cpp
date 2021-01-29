#include "precomp.h"

// Methods mainly derived from -> Monte Carlo Methods for Volumetric Light Transport Simulation: https://cs.dartmouth.edu/wjarosz/publications/novak18monte.pdf
// Also from https://giacomonazzaro.github.io/volume.html

// Phase function e.g. Henyey-Greenstein
// Where -1 < g < 1 is avg. cosine of the scattering directions and controls asymmetry of the phase function.
// g < 0 == backwards scattering.
// g > 0 == forward scattering.
float PhaseHG(float g, float cosTheta)
{
	const float inv4pi = 1 / ( 4 * PI );
	float denom = 1 + g * g + 2 * g * cosTheta;
	return inv4pi * ((1 - g * g) / (denom * sqrtf(denom)));
}

float PhaseHG(float g, vec3 wo, vec3 wi)
{
	return PhaseHG( g, wo.dot( wi ) );
}

vec3 SampleHG(float g, float e1, float e2)
{
	float s = 1.0 - 2.0 * e1;
	float cosTheta = ( s + 2.0 * g * g * g * ( -1.0 + e1 ) * e1 + g * g * s + 2.0 * g * ( 1.0 - e1 + e1 * e1 ) ) / ( ( 1.0 + g * s ) * ( 1.0 + g * s ) );
	float sinTheta = sqrt( 1.0 - cosTheta * cosTheta );
	return vec3( cos( 2.0 * PI * e2 ) * sinTheta, sin( 2.0 * PI * e2 ) * sinTheta, cosTheta );
}

float Transmittance(Ray &ray, float sigmaT)
{
	return exp(-sigmaT * min(ray.tMax * ray.direction.length(), MaxFloat));
}

inline double sampleSegment( double epsilon, float sigma, float smax )
{
	return -log( 1.0 - epsilon * ( 1.0 - exp( -sigma * smax ) ) ) / sigma;
}

inline void generateOrthoBasis( vec3 &u, vec3 &v, vec3 w )
{
	vec3 coVec = w;
	if ( fabs( w.x ) <= fabs( w.y ) )
		if ( fabs( w.x ) <= fabs( w.z ) )
			coVec = vec3( 0, -w.z, w.y );
		else
			coVec = vec3( -w.y, w.x, 0 );
	else if ( fabs( w.y ) <= fabs( w.z ) )
		coVec = vec3( -w.z, 0, w.x );
	else
		coVec = vec3( -w.y, w.x, 0 );
	coVec.normalize();
	u = w % coVec,
	v = w % u;
}

inline double scatter( const Ray &r, Ray *sRay, float sigma_s, double tin, float tout, double s )
{
	vec3 x = r.origin + r.direction * tin + r.direction * s;
	//Vec dir = sampleSphere(XORShift::frand(), XORShift::frand()); //Sample a direction ~ uniform phase function
	vec3 dir = SampleHG( -0.5, Rand( 1.0f ), Rand( 1.0f ) ); //Sample a direction ~ Henyey-Greenstein's phase function
	vec3 u, v;
	generateOrthoBasis( u, v, r.direction );
	dir = u * dir.x + v * dir.y + r.direction * dir.z;
	if ( sRay ) *sRay = Ray( x, dir, INFINITY, r.depth + 1 );
	return ( 1.0 - exp( -sigma_s * ( tout - tin ) ) );
}

color VolumetricPathTracer::Sample( Ray &ray, RayHit &hit )
{
	if ( ray.depth > maxDepth )
		return color( 0, 0, 0 );
	if ( Trace( ray, hit ) )
	{
		// 1. Test if same ray intersects a volume.
		// 2. Phase the ray (if phasing is > t then use surface, otherwise sample volume)
		// 3. If sample volume, scatter ray.
		float scale = 1.0, absorption = 1.0;
		shared_ptr<Material> mat = hit.material;
		color mCol = mat->GetColor( hit.uv );
		MaterialType mmat = mat->materialType;

		//for ( size_t i = 0; i < scene->volumes.size(); i++ )
		//{
		//	Ray vRay( ray );
		//	RayHit vHit;
		//	shared_ptr<HittableObject> volume = scene->volumes.at(i);
		//	if ( volume->Hit( vRay, vHit ) )
		//	{
		//		Ray sRay;
		//		RayHit sHit;
		//		float s = sampleSegment( Rand( 1.0f ), vHit.material->sigmaS, vHit.tFar - vRay.t );
		//		float ms = scatter( vRay, &sRay, vHit.material->sigmaS, vRay.t, vHit.tFar, s );
		//		float prob_s = ms;
		//		scale = 1.0 / ( 1.0 - prob_s );
		//		if (Rand(1.0) <= 0.001)
		//		{
		//			return Sample( sRay, sHit ) * ms * ( 1.0 / prob_s );
		//		}
		//		else
		//		{
		//			
		//		}
		//		if (ray.t >= vRay.t)
		//		{
		//			absorption = Transmittance( ray, mat->sigmaT() );
		//		}
		//	}
		//}

		if ( mmat == MaterialType::EMISSIVE )
		{
			return mCol * absorption;
		}
		if ( mmat == MaterialType::MIRROR )
		{
			Ray r( hit.point, reflect( ray.direction, hit.normal ) + ( 1.0f - hit.material->smoothness ) * RandomInsideUnitSphere(), INFINITY, ray.depth + 1 ); //new ray from intersection point
			return ( ( 1.0 - hit.material->specularity ) * mCol ) + ( ( hit.material->specularity ) * Sample( r, hit ) );									//Color of the material -> Albedo
		}
		if ( mmat == MaterialType::DIELECTRIC || mmat == MaterialType::GLASS )
		{
			return HandleDielectricMaterial( ray, hit );
		}

		color BRDF = mCol / PI;
		RayHit indirectHit, directHit;
		//color directColor = DirectIllumiation( ray, hit, BRDF, directHit );
		//color indirectColor = IndirectIllumination( ray, hit, BRDF, indirectHit );
		//return (directColor + indirectColor) / 2;
		if (Rand(1.0) <= 0.5f)
			return DirectIllumiation( ray, hit, BRDF, directHit );
		else
			return IndirectIllumination( ray, hit, BRDF, indirectHit );
	}
	return color( 0, 0, 0 );

	vec3 unit_direction = ray.direction;
	auto t = 0.5 * ( -unit_direction.y + 1.0 );
	color c = ( 1.0 - t ) * color( 1.0, 1.0, 1.0 ) + t * color( 0.5, 0.7, 1.0 );
	return c;
}

const color &VolumetricPathTracer::IndirectIllumination( Ray &ray, RayHit &hit, color &BRDF, RayHit &indirectHit )
{
	vec3 R = RandomInsideUnitSphere();
	if ( R.dot( hit.normal ) < 0.0 )
		R = -R;
	R = normalize( R );
	//new ray, start at intersection point, into random direction R
	point3 o = hit.point + hit.normal * EPSILON;
	Ray r( o, R, INFINITY, ray.depth + 1 );
	double ir = dot( hit.normal, R );
	color Ei = Sample( r, indirectHit ) * ( ir ); //irradiance is what you found with that new ray
	return TWO_PI * BRDF * Ei;
}

const color &VolumetricPathTracer::DirectIllumiation( Ray &ray, RayHit &hit, color &BRDF, RayHit &directHit )
{
	shared_ptr<HittableObject> randLight = scene->GetRandomEmissiveObject();
	point3 randPoint = randLight->GetRandomPoint();
	vec3 L = randPoint - hit.point;
	float dist = L.length();
	L = normalize( L );
	point3 o = hit.point + L * EPSILON;
	float tmax = dist - 2.0f * EPSILON;
	Ray r( o, L, INFINITY, maxDepth );

	if ( Trace( r, directHit ) )
	{
		if (directHit.hitObject == randLight)
		{
			vec3 lightNormal = directHit.normal;
			float cosO = ( -L ).dot( lightNormal );
			float cosI = L.dot( hit.normal );
			if ( ( cosO <= 0 ) || ( cosI <= 0 ) )
				return color( 0, 0, 0 );
			float solidAngle = ( cosO * randLight->GetArea() / ( dist * dist ) );
			return BRDF * (double)scene->emissiveObjects.size() * randLight->material->albedo * solidAngle * cosI;
		}
	}

	return color( 0, 0, 0 );
}
