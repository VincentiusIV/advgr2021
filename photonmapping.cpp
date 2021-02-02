
#include "precomp.h"


//initial values
int nrPhotons = 2000; //TODO: eventually light sensitive (now its 100 per light)
int nrBounces = 6;	 //cap on amount of bounces (photon is stopped by RR or this)
int estimateP = 20;	 //the photon estimate in the radiance around point x
int estimateR = 1;	 //the radiance around point x with photon estimate

//you can store a photon multiple times
int maxStack = nrPhotons * nrBounces; //Q: * nr of lights?
int topStack = -1;
photon *photonmap = new photon[maxStack];

//stack pusher for photonmap 
void PhotonMap::push( photon i )
{
	if (topStack < maxStack - 1)
	{
		photonmap[++topStack] = i;
	}
}

//Q: is survive/die correct?
bool PhotonMap::RussianRoulette(color &mCol)
{
	float pSurvival = clamp( fmax( mCol.x, fmax( mCol.y, mCol.z ) ), 0.0f, 1.0f ); 
	if ( pSurvival < Rand( 1.0 ) )
	{
		return false; //dies
	}
	else
		return true; //survives
}

//TODO: need a global photon map & caustic photon map

//HINT: difference between ray tracing and photon tracing: photons propagate flux while rays gather radiance.

//Q: do I have to add &? behind the things???

// PHASE 1

//photon emittance from a light
void PhotonMap::photonEmittanceLight(Ray ray ) 
{
	//for ( int i = 0; i < scene->lights.size(); i++ )
	//{
	shared_ptr<HittableObject> randLight = scene->GetRandomEmissiveObject(); //TODO: is only one light

	for ( int i = 0; i < nrPhotons; i++ )
	{
		photon photon;
		RayHit hit;

		point3 randPointL = randLight->GetRandomPoint();
		vec3 lightNormal = randLight->GetNormalAtPoint( randPointL );
		
		vec3 R = RandomInsideUnitSphere(); //Q: is this whole sphere or hemisphere?
		if ( R.dot( lightNormal ) < 0.0 )
			R = -R;
		R = normalize( R );
		
		photon.power = ( 1, 1, 1 );
		photon.position = randPointL;
		photon.L = R;
					
		//Send photon in random direction
		Ray r( randPointL, R, INFINITY, ray.depth + 1 ); //Q: Can I just use rays???
		//Trace the photon
		SampleP( r, hit,photon );
	}
	//}
}

//photon emittance from a surface
void PhotonMap::photonEmittance( RayHit hit, photon photon, Ray ray)
{
	vec3 R = RandomInsideUnitSphere(); //Q: is this whole sphere or hemisphere?
	if ( R.dot( hit.normal ) < 0.0 )
		R = -R;
	R = normalize( R );

	//Send photon in random direction
	Ray r( hit.point, R, INFINITY, ray.depth + 1 );
	//Trace photon
	SampleP( r, hit,photon );
}

//See if photon hits surface -> yes, store in photonmap
void PhotonMap::SampleP(Ray &ray, RayHit &hit, photon photon)
{
	//RayHit hit;
	if (Trace (ray, hit))
	{
			shared_ptr<Material> mat = hit.material;
			color mCol = mat->GetColor( hit.uv );
			MaterialType mmat = mat->materialType;

			if (mmat != MaterialType::MIRROR)
			{
				photon.position = hit.point;
				photon.power = mCol;		//Q: is this correct color?/energy?
				photon.L = ray.direction;	//Q: is this one correct? or should it go away from the surface?
				//store the location, energy and incident direction in photonmap
				push(photon) ; 

				if (!RussianRoulette(mCol))
				{
					return; 
				}
				//Q: is this? -> send random direction: randomly chosen vector that is above the intersection point with a probability proportioinal to the cosine of the angle with the normal (also done with RR?)
				if (ray.depth <= nrBounces)
				{
					photonEmittance( hit, photon, ray );
					return;
				}
					
			}
			if (mmat == MaterialType::MIRROR)
			{
				//TODO: check: photon is reflected with its intensity scaled by the reflection index of that object.
				//Q: how above thing?
				//Q: is ray correct reflected below?
				Ray r( hit.point, reflect( ray.direction, hit.normal ) + ( 1.0f - hit.material->smoothness ) * RandomInsideUnitSphere(), INFINITY, ray.depth + 1 ); //new ray from intersection point
					
				//Q: do we need to change photon when reflecting? ex. red mirror?
				//photon.power *= mCol; (?)
			    //( ( 1.0 - hit.material->specularity ) * mCol ) + ( ( hit.material->specularity ) *
			    SampleP( r, hit, photon ) ;		//Color of the material -> Albedo		
				return;
			}
	}
	return;
}

//TODO: sort (and add) the kD-tree
//TODO: add shadow rays(?) for Caustic map (second intersection a shadow photon is stored instead of photon)

/*
//Q: can I use this to build kD-tree???
function Buildtree(photons P, voxel V)
{
	if(Terminate(P,V)) return new LeafNode(P)
	plane pla = FindPlane(P,V)
	Voxel Vl, Vr = Split V with pla
	photons Pl = { p el P | (p omgekeerdeU Vl)=!0
	photons Pr = { p el P | (p omgekeerdeU Vr)=!0
	return new InteriorNode(pla, Build(Pl,Vl), Build(Pr,Vr))
}
function BuildkDtree(photons P)
{
	Voxel V = bounds(P)
	return Buildtree(P, V)
}*/

//HINT: 2 ways
//1. Take a fixed size sphere and get all photons inside that sphere for estimate + weights
//2. Take N photons closest to x and find the given radius r ->pi*r^2

//HINT: radiance estimation -> at each non-specular path vertex we estimate the reflected radiance

// PHASE 2

//TODO: add pathtracer sample, vervang diffuse met photonDensity (ipv licht)

color PhotonMap::Sample( Ray &ray, RayHit &hit )
{
	color BRDF;
	if ( ray.depth > maxDepth )
		return color( 0, 0, 0 );
	if ( Trace( ray, hit ) )
	{
		shared_ptr<Material> mat = hit.material;
		color mCol = mat->GetColor( hit.uv );
		MaterialType mmat = mat->materialType;

		if ( mmat == MaterialType::EMISSIVE )
		{
			//return photonDensity( ray, hit, mCol ); // ?? mCol;
			return mCol;							
		}
		if ( mmat == MaterialType::MIRROR )
		{
			Ray r( hit.point, reflect( ray.direction, hit.normal ) + ( 1.0f - hit.material->smoothness ) * RandomInsideUnitSphere(), INFINITY, ray.depth + 1 ); //new ray from intersection point
			return ( ( 1.0 - hit.material->specularity ) * mCol ) + ( ( hit.material->specularity ) * RayTracer::Sample( r ) );									//Color of the material -> Albedo
		}
		if ( mmat == MaterialType::DIELECTRIC || mmat == MaterialType::GLASS )
		{
			return HandleDielectricMaterial( ray, hit );
		}

		BRDF = mCol / PI;

		return photonDensity(ray, hit, BRDF);
	}
	//return color( 0, 0, 0 );
	vec3 unit_direction = ray.direction;
	auto t = 0.5 * ( -unit_direction.y + 1.0 );
	color c = ( 1.0 - t ) * color( 1.0, 1.0, 1.0 ) + t * color( 0.5, 0.7, 1.0 );
	return c;
}


 color PhotonMap::photonDensity( Ray &ray, RayHit hit, color BRDF )
{
	//RayHit hit;
	color energy = (1,0,0);  //surface starts with 0 energy
	for (int i = 0; i < maxStack; i++)
	{
		//TODO: this doesn't check if inside surface...
		float insideR = pow( ( photonmap[i].position.x - hit.point.x ), 2 ) + pow( ( photonmap[i].position.y - hit.point.y ), 2 ) + pow( ( photonmap[i].position.z - hit.point.z ), 2 );
		
		//inside 'density sphere'
		if (insideR < pow(estimateR,2))
		{
			//filter from slides
			float weightfilter = 0.918 * fabs( 1 - (1-exp(-1.953* (insideR/2*pow(estimateR,2) )) ) / (1-exp(-1.953) ) ); 
		    //max( 0.0f,(1 - sqrt( insideR )/(1*estimateR))); //Q: correct??

			//add to energy
			//light equation: brdf * flux(?) *weightfilter;
			energy += BRDF * photonmap[i].power *weightfilter;
		}
	}
	return energy;
}

