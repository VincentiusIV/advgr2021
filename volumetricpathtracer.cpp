#include "precomp.h"

// Methods mainly derived from -> Physically Based Rendering: From Theory to Implementation
// Also from https://giacomonazzaro.github.io/volume.html

// Source: Physically Based Rendering: From Theory to Implementation
inline void CoordinateSystem( const vec3 &v1, vec3 *v2, vec3 *v3 )
{
	if ( std::abs( v1.x ) > std::abs( v1.y ) )
		*v2 = vec3( -v1.z, 0, v1.x ) / std::sqrt( v1.x * v1.x + v1.z * v1.z );
	else
		*v2 = vec3( 0, v1.z, -v1.y ) / std::sqrt( v1.y * v1.y + v1.z * v1.z );
	*v3 = cross( v1, *v2 );
}

// Source: Physically Based Rendering: From Theory to Implementation
inline vec3 SphericalDirection( float sinTheta, float cosTheta, float phi, const vec3 &x, const vec3 &y, const vec3 &z )
{
	return sinTheta * std::cos( phi ) * x + sinTheta * std::sin( phi ) * y + cosTheta * z;
}

// Henyey-Greenstein Phase function 
// Where -1 < g < 1 is avg. cosine of the scattering directions and controls asymmetry of the phase function.
// g < 0 == backwards scattering.
// g > 0 == forward scattering.
inline float PhaseHG( float g, float cosTheta )
{
	const float inv4pi = 1 / ( 4 * PI );
	float denom = 1 + g * g + 2 * g * cosTheta;
	return inv4pi * ( ( 1 - g * g ) / ( denom * sqrtf( denom ) ) );
}

inline float SampleHG( vec3 wo, vec3 *wi, float e1, float e2, float g )
{
	float cosTheta;
	if (abs(g) < 1e-3)
	{
		cosTheta = 1 - 2 * e1;
	}
	else
	{
		float sqrTerm = ( 1 - g * g ) / ( 1 + g - 2 * g * e1 );
		cosTheta = -( 1 + g * g - sqrTerm * sqrTerm ) / ( 2 * g );
	}
	float sinTheta = sqrtf( fmax( 0.0f, 1.0f - cosTheta * cosTheta ) );
	float phi = 2.0f * PI * e2;
	vec3 v1, v2;
	CoordinateSystem( wo, &v1, &v2 );
	*wi = SphericalDirection( sinTheta, cosTheta, phi, v1, v2, wo );
	return PhaseHG( g, cosTheta );
}

inline float Transmittance( Ray &ray, float sigmaT, float t )
{
	return exp(-sigmaT * min(t, MaxFloat));
}

// TODO: Use this to create simple spatial densities. Should be in volume?
static siv::PerlinNoise perlin;

color VolumetricPathTracer::Sample( Ray &r, RayHit &h )
{
	color beta( 1.0 );
	Ray ray( r ), vRay(r);
	for ( int bounceIdx = 0; bounceIdx < maxDepth; bounceIdx++ )
	{
		RayHit hit;
		bool foundIntersection = Trace(ray, hit);
		if (hit.intersectsVolume)
		{
			shared_ptr<Material> volumeMat = hit.volume->material;
			float density = volumeMat->g; 
			float scatterDist = abs(log( Rand( 1.0 ) )) / volumeMat->sigmaS;
			if ( scatterDist < ray.t )
			{
				if ( Rand( 1.0 ) < volumeMat->volumeAlbedo() )
				{
					// continue through volume
					//vec3 wi = RandomInsideUnitSphere(); // scatter in random directions.
					vec3 wi;
					SampleHG( -ray.direction, &wi, Rand( 1.0f ), Rand( 1.0f ), density );
					ray = Ray( ray.At(scatterDist), wi, INFINITY, 0 );
					continue;	
				}
				else
				{
					// scatter against volume
					hit.hitObject = hit.volume;
					hit.point = ray.At(scatterDist);
					hit.normal = RandomInsideUnitSphere();
					if ( hit.normal.dot( ray.direction ) > 0.0f )
						hit.normal = -hit.normal;
					hit.material = hit.volume->material;
					foundIntersection = true;
				}	

				beta *= Transmittance( ray, hit.material->sigmaT(), scatterDist );				
			}	
		}

		if ( !foundIntersection )
		{
			beta *= 0.0f;
			break;
		}

		shared_ptr<Material> mat = hit.material;
		color mCol = mat->GetColor( hit.uv );
		MaterialType mmat = mat->materialType;

		if ( mmat == MaterialType::EMISSIVE )
		{
			beta *= mat->emission;
			break;
		}
		if ( mmat == MaterialType::MIRROR )
		{
			ray = Ray( hit.point, reflect( ray.direction, hit.normal ), INFINITY, ray.depth + 1 );
			continue;
		}
		if ( mmat == MaterialType::DIELECTRIC || mmat == MaterialType::GLASS )
		{
			float cosi = fmax( -1.0, fmin( 1.0, dot( ray.direction, hit.normal ) ) );
			float cosTheta2 = cosi * cosi;
			float etai = 1.0, etat = hit.material->n;
			vec3 n = hit.normal;
			if ( cosi < 0 )
			{
				cosi = -cosi;
			}
			else
			{
				std::swap( etai, etat );
				n = -hit.normal;
			}
			float etaiOverEtat = etai / etat;
			float k = 1.0f - etaiOverEtat * etaiOverEtat * ( 1.0 - cosTheta2 );
			float sinTheta = etaiOverEtat * sqrtf( max( 0.0, 1.0 - cosTheta2 ) );
			float reflectance;
			Fresnel( sinTheta, reflectance, cosi, etat, etai );
			float transmittance = ( 1.0f - reflectance );
			float reflectChance = reflectance * Rand( 1.0f );
			vec3 dir = k < 0 ? 0.0f : etaiOverEtat * ray.direction + ( etaiOverEtat * cosi - sqrtf( k ) ) * n;
			point3 refractRayOrigin = hit.isFrontFace ? hit.point - hit.normal * 0.001 : hit.point + hit.normal * 0.001f;

			Ray reflectRay( hit.point, reflect( ray.direction, hit.normal ), INFINITY, ray.depth + 1 );
			if (Rand(1.0f) < 0.5f)
			{
				ray = reflectRay;
				beta *= reflectance;
			}
			else
			{
				ray = Ray( refractRayOrigin, dir, INFINITY, ray.depth + 1.0 );
				beta *= transmittance;
			}
			// Scale up color by 2 since we perform RR on reflect/transmit rays.
			beta *= 2.0f;
			continue;
		}
		else if ( mmat == MaterialType::DIFFUSE || mmat == MaterialType::VOLUMETRIC )
		{
			// BRDF scaled up by 2 since we perform RR on Direct/Indirect lighting.
			color BRDF = mCol * INV_PI * 2.0;

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