#include "precomp.h"

color PathTracer::Sample(Ray ray, Scene *scene)
{
	//initials
	color BRDF; //should probably be outside function
	RayHit hit;

	if (Trace(scene, ray, hit))
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
			return mCol * Sample( r, scene ); //Color of the material -> Albedo
		}
		if (mmat == MaterialType::DIELECTRIC)
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
			vec3 dir = k < 0 ? 0.0f : etaiOverEtat * ray.direction + ( etaiOverEtat * cosi - sqrtf( k ) ) * n;
			point3 refractRayOrigin = hit.isFrontFace ? hit.point - hit.normal * 0.001 : hit.point + hit.normal * 0.001f;


			float sinTheta = etaiOverEtat * sqrtf( max( 0.0, 1.0 - cosTheta2 ) );
			float reflectance;
			Fresnel( sinTheta, reflectance, cosi, etat, etai );
			float transmittance = ( 1.0f - reflectance );

			float reflectChance = reflectance * Rand( 1.0f );
			float refractChance = transmittance * Rand( 1.0f );

			//only different code than in the witted raytracer.
			if (reflectChance > refractChance)
			{
				point3 p = hit.point;
				vec3 r = reflect( ray.direction, hit.normal );	
				Ray reflectRay( p, r , INFINITY, ray.depth + 1 );
				return Sample( reflectRay, scene );
			}
			else
			{
				return Sample( Ray( refractRayOrigin, dir, INFINITY, ray.depth + 1.0 ), scene );
			}
		}
		

		BRDF = mCol / PI; //albedo -> color of the material

		if ( ray.depth > maxDepth ) //reasonable number = 7
			return color(0,0,0);

		//continue in random direction on your hemisphere
		vec3 R = RandomInsideUnitSphere(); 
		if ( R.dot( hit.normal ) <= 0.0  )
			R = -R;

		//new ray, start at intersection point, into random direction R
		Ray r( hit.point + hit.normal * 0.001f, R, INFINITY, ray.depth + 1 ); 

		float ir = dot( hit.normal, R );
		color Ei = Sample( r, scene ) * ( ir ); //irradiance is what you found with that new ray
		return PI * 2.0f * BRDF * Ei;
	}
	else //no hit, ray left the scene. return black
	{
		return color( 0, 0, 0 );
	}
}

// Vec radiance( const Ray &r, int depth, unsigned short *Xi )
//{
//	double t;									// distance to intersection
//	int id = 0;									// id of intersected object
//	if ( !intersect( r, t, id ) ) return Vec(); // if miss, return black
//	const Sphere &obj = spheres[id];			// the hit object
//	Vec x = r.o + r.d * t, n = ( x - obj.p ).norm(), nl = n.dot( r.d ) < 0 ? n : n * -1, f = obj.c;
//	double p = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y : f.z; // max refl
//	if ( ++depth > 5 )
//		if ( erand48( Xi ) < p )
//			f = f * ( 1 / p );
//		else
//			return obj.e; //R.R.
//	if ( obj.refl == DIFF )
//	{ // Ideal DIFFUSE reflection
//		double r1 = 2 * M_PI * Rand( Xi );
//		double r2 = Rand( Xi );
//		double r2s = sqrt( r2 );
//		vec3 w = nl;
//		vec3 u = normalize ( ( fabs( w.x ) > .1 ? vec3( 0, 1 ) : vec3( 1 ) ) % w );
//		vec3 v = w % u;
//		vec3 d = normalize( u * cos( r1 ) * r2s + v * sin( r1 ) * r2s + w * sqrt( 1 - r2 ) );
//		return obj.emission + f.mult( radiance( Ray( x, d ), depth, Xi ) );
//	}
//	else if ( obj.refl == SPEC ) // Ideal SPECULAR reflection
//		return obj.e + f.mult( radiance( Ray( x, r.d - n * 2 * n.dot( r.d ) ), depth, Xi ) );
//	Ray reflRay( x, r.d - n * 2 * n.dot( r.d ) ); // Ideal dielectric REFRACTION
//	bool into = n.dot( nl ) > 0;				  // Ray from outside going in?
//	double nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = r.d.dot( nl ), cos2t;
//	if ( ( cos2t = 1 - nnt * nnt * ( 1 - ddn * ddn ) ) < 0 ) // Total internal reflection
//		return obj.e + f.mult( radiance( reflRay, depth, Xi ) );
//	Vec tdir = ( r.d * nnt - n * ( ( into ? 1 : -1 ) * ( ddn * nnt + sqrt( cos2t ) ) ) ).norm();
//	double a = nt - nc, b = nt + nc, R0 = a * a / ( b * b ), c = 1 - ( into ? -ddn : tdir.dot( n ) );
//	double Re = R0 + ( 1 - R0 ) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / ( 1 - P );
//	return obj.e + f.mult( depth > 2 ? ( erand48( Xi ) < P ? // Russian roulette
//				 radiance( reflRay, depth, Xi ) * RP
//			   : radiance( Ray( x, tdir ), depth, Xi ) * TP )
//				: radiance( reflRay, depth, Xi ) * Re + radiance( Ray( x, tdir ), depth, Xi ) * Tr );
//} 


bool PathTracer::Trace(Scene *scene, Ray ray, RayHit &hit ) 
{
	bool hitAny = false;
	for ( size_t i = 0; i < scene->objects.size(); i++ )
	{
		shared_ptr<HittableObject> obj = scene->objects.at( i );
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
