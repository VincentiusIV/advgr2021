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

color VolumetricPathTracer::Sample( Ray &r, RayHit &h )
{
	color beta( 1.0 );
	Ray ray( r ), vRay(r);
	bool specularBounce = false;
	for ( int bounceIdx = 0; bounceIdx < maxDepth; bounceIdx++ )
	{
		RayHit hit;
		bool foundIntersection = Trace(ray, hit);

		if (hit.hitVolume)
		{
			// 2. Phase the ray (if phasing is > t then use surface, otherwise sample volume)
			// 3. If sample volume, scatter ray.
		}

		float scale = 1.0, absorption = 1.0;
		shared_ptr<Material> mat = hit.material;
		color mCol = mat->GetColor( hit.uv );
		MaterialType mmat = mat->materialType;
		if ( !foundIntersection )
		{
			beta *= 0.0f;
			break;
		}
		if ( mmat == MaterialType::EMISSIVE )
		{
			beta *= mCol;
			break;
		}
		//if ( mmat == MaterialType::MIRROR )
		//{
		//	Ray r( hit.point, reflect( ray.direction, hit.normal ) + ( 1.0f - hit.material->smoothness ) * RandomInsideUnitSphere(), INFINITY, ray.depth + 1 ); //new ray from intersection point
		//	beta *= ( ( 1.0 - hit.material->specularity ) * mCol ) + ( ( hit.material->specularity ) * Sample( r, hit ) );										//Color of the material -> Albedo
		//}
		//if ( mmat == MaterialType::DIELECTRIC || mmat == MaterialType::GLASS )
		//{
		//	beta *= HandleDielectricMaterial( ray, hit );
		//}
		else if ( mmat == MaterialType::DIFFUSE )
		{
			color BRDF = mCol * INV_PI;

			// Indirect
			if ( Rand( 1.0f ) < 0.5f )
			{
				vec3 R = RandomInsideUnitSphere();
				if ( R.dot( hit.normal ) < 0.0 )
					R = -R;
				R = normalize( R );
				point3 o = hit.point + hit.normal * EPSILON;
				double ir = dot( hit.normal, R );
				beta *= TWO_PI * BRDF * ir;
				ray = Ray( o, R, INFINITY, ray.depth + 1 );
			}
			// Direct
			else
			{
				shared_ptr<HittableObject> randLight = scene->GetRandomEmissiveObject();
				point3 randPoint = randLight->GetRandomPoint();
				vec3 L = randPoint - hit.point;
				float dist = L.length();
				L = normalize( L );
				point3 so = hit.point + L * EPSILON;
				float tmax = dist - 2.0f * EPSILON;
				Ray shadowRay( so, L, INFINITY, maxDepth );
				RayHit shadowHit;
				color directColor;
				bool direct = false;
				if ( Trace( shadowRay, shadowHit ) )
				{
					if ( shadowHit.hitObject == randLight )
					{
						vec3 lightNormal = shadowHit.normal;
						float cosO = ( -L ).dot( lightNormal );
						float cosI = L.dot( hit.normal );
						if ( !( ( cosO <= 0 ) || ( cosI <= 0 ) ) )
						{
							float solidAngle = ( cosO * randLight->GetArea() / ( dist * dist ) );
							beta *= BRDF * (double)scene->emissiveObjects.size() * randLight->material->albedo * solidAngle * cosI;
							direct = true;
						}
					}
				}

				if ( !direct )
					beta *= 0.0f;
				break;
			}
		}
		else
		{
			beta *= 0.0f;
			break;
		}
		
	}

	return beta;
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
