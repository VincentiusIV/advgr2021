#pragma once
#include "precomp.h"
using namespace std;

struct photon
{
  public:
	point3 position;	// world space position of the photon hit
	point3 power;	//current power level for the photon (can also be stored in float 3)
	vec3 L;			//incident direction
};

//HINT:
//in raytracer/pathtracer/game:
//if(usePhoton ==true) //lighting via Photon mapping
//rgb = gatherPhotons(gPoint, gType, gIndex);
//else //lighting via standard illumination
//return rgb;


//Q: move : public RayTracer to .h?
class PhotonMap : public RayTracer 
{
  public:

	//initial values
	int nrPhotons = 100; //TODO: eventually light sensitive (now its 100 per light)
	int nrBounces = 3;	 //Q: eventually replace by Roussian Roulette (?) or more bounces(?)
	int estimateP = 20;  //the photon estimate in the radiance around point x
	int estimateR = 3;   //the radiance around point x with photon estimate
	
	int maxStack = nrPhotons * nrBounces; //you can store a photon multiple times
	int topStack = -1;
	photon *photonmap = new photon[maxStack]; 

	//stack pusher for photonmap 
	void push( photon i )
	{
		if (topStack < maxStack - 1)
		{
			photonmap[++topStack] = i;
		}
	}

	bool RussianRoulette(color &mCol)
	{
		float pSurvival = Clamp( fmax( mCol.x, fmax( mCol.y, mCol.z ) ), 0.0f, 1.0f ); //TODO: I don't understand pointers or this errors 
		if ( pSurvival < Rand( 1.0 ) )
		{
			return true; //survives
		}
		else
			return false; //dies
	}

	//TODO: need a global photon map & caustic photon map

	//HINT: difference between ray tracing and photon tracing: photons propagate flux while rays gather radiance.

	//Q: do I have to add &? behind the things???

	//photon emittance from a light
	void photonEmittanceLight( Ray ray ) //TODO: take a light or put for loop
	{
		//TODO: loop over ALL lights, not just one
		shared_ptr<HittableObject> randLight = scene->GetRandomEmissiveObject(); //TODO: however we don't want randomlight, we wanna loop over all
		for (int i = 0; i < nrPhotons; i++)
		{
			photon photon; 
			
			point3 randPointL = randLight->GetRandomPoint();
			vec3 lightNormal = randLight->GetNormalAtPoint( randPointL );

			vec3 R = RandomInsideUnitSphere(); //Q: is this whole sphere or hemisphere?
			if ( R.dot( lightNormal ) < 0.0 )
				R = -R;
			R = normalize( R );

			//Q: Do I need to store photon at light as well?
			//photon.position = randPointL;
			//photon.power = ( 1, 1, 1 ); //TODO: color of light
			//photon.L = R; 

			//Send photon in random direction
			Ray r( randPointL, R, INFINITY, ray.depth + 1 ); //Q: Can I just use rays???
			//Trace the photon
			SamplePhoton( r, photon );
		}

	}

	//photon emittance from a surface
	void photonEmittance( RayHit hit, photon photon, Ray ray)
	{
		vec3 R = RandomInsideUnitSphere(); //Q: is this whole sphere or hemisphere?
		if ( R.dot( hit.normal ) < 0.0 )
			R = -R;
		R = normalize( R );

		//Send photon in random direction
		Ray r( hit.point, R, INFINITY, ray.depth + 1 );
		//Trace photon
		SamplePhoton( r, photon );
	}

	//See if photon hits surface -> yes, store in photonmap
	void SamplePhoton(Ray &ray, photon photon)
	{
		RayHit hit;
		if (Trace (ray, hit))
		{
				shared_ptr<Material> mat = hit.material;
				color mCol = mat->GetColor( hit.uv );
				MaterialType mmat = mat->materialType;

				if (mmat != MaterialType::MIRROR)
				{
					photon.position = hit.point;
					photon.power = mCol; //Q: is this correct color?
					photon.L = ray.direction; //Q: is this one correct? or should it go away from the surface?
					//store the location, energy and incident direction in photonmap
					push(photon) ; 

					if (!RussianRoulette(mCol))
					{
						return; //Q: what return when photon has died? black? nothing?
					}
					//Q: is this? -> send random direction: randomly chosen vector that is above the intersection point with a probability proportioinal to the cosine of the angle with the normal (also done with RR?)
					if (ray.depth <= nrBounces)
					{
						photonEmittance( hit, photon, ray );
					}
					
				}
				if (mmat == MaterialType::MIRROR)
				{
					//TODO: check: photon is reflected with its intensity scaled by the reflection index of that object.
					//Q: how above thing?
					//Q: is ray correct reflected below?
					Ray r( hit.point, reflect( ray.direction, hit.normal ) + ( 1.0f - hit.material->smoothness ) * RandomInsideUnitSphere(), INFINITY, ray.depth + 1 ); //new ray from intersection point
					
					//Q: do we need to change photon when reflecting? ex. red mirror?
				    //( ( 1.0 - hit.material->specularity ) * mCol ) + ( ( hit.material->specularity ) *
				    SamplePhoton( r, photon ) ;		//Color of the material -> Albedo		
					return;
				}
		}
		return;
	}

	//TODO: sort (and add) the kD-tree

	//TODO: add shadow rays(?) for Caustic map (second intersection a shadow photon is stored instead of photon)

	//phase 1
	//photons traced trough scene with pathtracer
	//everytime it hits a surface ->stored in photon map and russian roulette is applied
	//Q: is this? -> the new direction of the reflected photon is computed using brdf of the surface
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

	color photonDensity(Ray &ray, RayHit hit, color BRDF)
	{
		//RayHit hit;
		color energy;  //surface starts with 0 energy
		for (int i = 0; i < maxStack; i++)
		{
			//TODO: this doesn't check if inside surface...
			float insideR = pow( ( photonmap[i].position.x - hit.point.x ), 2 ) + pow( ( photonmap[i].position.y - hit.point.y ), 2 ) + pow( ( photonmap[i].position.z - hit.point.z ), 2 );
			
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


	//phase 2
	//radiance estimation -> at each non-specular path vertex we estimate the reflected radiance
};