#include "precomp.h"

color PathTracer::Sample(Ray ray)
{
	//initials
	color BRDF; //should probably be outside function
	RayHit hit;
	if ( ray.depth > maxDepth ) //reasonable number = 7
		return color( 0, 0, 0 );
	if (Trace(ray, hit))
	{
		shared_ptr<Material> mat = hit.material;
		color mCol = mat->color;
		MaterialType mmat = mat->materialType;

		if ( mmat == MaterialType::EMISSIVE )
		{
			return mCol; //return color of light source. This will be the 'material' color.
		}
		if ( mmat == MaterialType::MIRROR )
		{
			Ray r( hit.point, reflect( ray.direction, hit.normal ) + ( 1.0f - hit.material->smoothness ) * RandomInsideUnitSphere(), INFINITY, ray.depth + 1 ); //new ray from intersection point
			return mCol * Sample( r ); //Color of the material -> Albedo
		}
		if (mmat == MaterialType::DIELECTRIC || mmat == MaterialType::GLASS)
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
			float refractChance = transmittance * Rand( 1.0f );

			//only different code than in the witted raytracer.
			if (reflectChance > refractChance)
			{
				Ray reflectRay( hit.point, reflect( ray.direction, hit.normal ), INFINITY, ray.depth + 1 );
				return Sample( reflectRay );
			}
			else
			{
				vec3 dir = k < 0 ? 0.0f : etaiOverEtat * ray.direction + ( etaiOverEtat * cosi - sqrtf( k ) ) * n;
				point3 refractRayOrigin = hit.isFrontFace ? hit.point - hit.normal * 0.001 : hit.point + hit.normal * 0.001f;
				return Sample( Ray( refractRayOrigin, dir, INFINITY, ray.depth + 1.0 ) );
			}
		}


		BRDF = mCol / PI;
			return LitMethod2( ray, hit, BRDF );
		//if ( Rand( 1.0 ) > 0.5f )
		//else 
		//	return LitMethod2( scene, ray, hit, BRDF );
	}
	return color( 0, 0, 0 );
}

const color &PathTracer::LitMethod2( Ray &ray, RayHit &hit, color &BRDF )
{
	shared_ptr<HittableObject> randLight = scene->GetRandomEmissiveObject();
	point3 randPoint = randLight->GetRandomPoint();
	vec3 L = randPoint - hit.point;
	float dist = L.length();
	L = L / dist;
	vec3 lightNormal = randLight->GetNormalAtPoint( randPoint );
	float cosO = ( -L ).dot( lightNormal );
	float cosI = L.dot( hit.normal );
	if ( ( cosO <= 0 ) || ( cosI <= 0 ) )
		return color( 0, 0, 0 );
	point3 o = hit.point + L * EPSILON;
	float tmax = dist - 2.0f * EPSILON;
	Ray r( o, L, tmax, 7 );
	if ( !Trace( r, hit, MaterialType::EMISSIVE) )
	{
		float solidAngle = ( cosO * randLight->GetArea() / ( dist * dist ) );
		return BRDF * (double)scene->emissiveObjects.size() * randLight->material->color * solidAngle * cosI;
	}
	else
	{
		return color( 0, 0, 0 );
	}
}

const color &PathTracer::LitMethod1( Ray &ray, RayHit &hit, color &BRDF )
{
	vec3 R = RandomInsideUnitSphere();
	if ( R.dot( hit.normal ) <= 0.0 )
		R = -R;
	//new ray, start at intersection point, into random direction R
	point3 o = hit.point + hit.normal * EPSILON;
	Ray r(o, R, INFINITY, ray.depth + 1 );
	float ir = dot( hit.normal, R );
	color Ei = Sample( r ) * ( ir ); //irradiance is what you found with that new ray
	return TWO_PI * BRDF * Ei;
}

bool PathTracer::Trace(Ray &ray, RayHit & hit ) 
{
	return Trace( ray, hit, MaterialType::DIFFUSE );
}

bool PathTracer::Trace( Ray &ray, RayHit &hit, MaterialType typeToIgnore )
{
	bool hitAny = false;
	for ( size_t i = 0; i < scene->objects.size(); i++ )
	{
		shared_ptr<HittableObject> obj = scene->objects.at( i );
		if (typeToIgnore != MaterialType::DIFFUSE && obj->material->materialType == typeToIgnore)
			continue;
		hitAny |= ( obj->Hit( ray, hit ) );
	}
	return hitAny;
}

void PathTracer::Fresnel( float sinTheta, float &reflectance, float &cosi, float etat, float etai )
{
	if ( sinTheta >= 1.f )
	{
		reflectance = 1.0f;
	}
	else
	{
		float cost = sqrtf( max( 0.0f, 1.0f - sinTheta * sinTheta ) );
		cosi = fabsf( cosi );
		float rs = ( ( etat * cosi ) - ( etai * cost ) ) / ( ( etat * cosi ) + ( etai * cost ) );
		float rp = ( ( etai * cosi ) - ( etat * cost ) ) / ( ( etai * cosi ) + ( etat * cost ) );
		reflectance = 0.5f * ( rs * rs + rp * rp );
	}
}
