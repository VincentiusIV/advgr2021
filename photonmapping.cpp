
#include "precomp.h"


//initial values
int nrPhotons = 2000;  
int nrBounces = 6;	 //cap on amount of bounces (photon is stopped by RR or this)
int estimateP = 20;	 //the photon estimate in the radiance around point x
int estimateR = 1;	 //the radiance around point x with photon estimate
float estimateRC = 1.2;

//you can store a photon multiple times
int maxStack = nrPhotons * nrBounces; 
int topStackP = -1;
int topStackC = -1;
photon *photonmap = new photon[maxStack];
photon *causticmap = new photon[maxStack]; //send specific to glass ball

//stack pusher for photonmap 
void PhotonMap::push(photon arr[], photon i )
{
	if (arr == photonmap)
	{
		if ( topStackP < maxStack - 1 )
		{
			arr[++topStackP] = i;
		}
	}
	if ( arr == causticmap )
	{
		if ( topStackC < maxStack - 1 )
		{
			arr[++topStackC] = i;
		}
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

//Q: do I have to add &? behind the things???

// -- PHASE 1 --

//photon emittance from a light
void PhotonMap::photonEmittanceLight(Ray ray ) 
{	
	shared_ptr<HittableObject> randLight = scene->GetRandomEmissiveObject(); //only one light

	for ( int i = 0; i < nrPhotons; i++ )
	{
		photon photon;
		RayHit hit;

		point3 randPointL = randLight->GetRandomPoint();
		vec3 lightNormal = randLight->GetNormalAtPoint( randPointL );
		
		vec3 R = RandomInsideUnitSphere();
		if ( R.dot( lightNormal ) < 0.0 )
			R = -R;
		R = normalize( R );
		
		photon.power = ( 1, 1, 1 );
		photon.position = randPointL;
		photon.L = R;
					
		//Send photon in random direction
		Ray r( randPointL, R, INFINITY, ray.depth + 1 ); 
		//Trace the photon
		SampleP( r, hit,photon );
	}
}

//photon emittance from a surface
void PhotonMap::photonEmittance( RayHit hit, photon photon, Ray ray)
{
	vec3 R = RandomInsideUnitSphere();
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

	if (Trace (ray, hit))
	{
			shared_ptr<Material> mat = hit.material;
			color mCol = mat->GetColor( hit.uv );
			MaterialType mmat = mat->materialType;

			if (mmat == MaterialType::DIFFUSE)
			{
				photon.position = hit.point;
				photon.power = mCol;	  
				photon.L = ray.direction;	
				
				push(photonmap, photon) ; 

				if (!RussianRoulette(mCol))
				{
					return; 
				}
				
				if (ray.depth <= nrBounces)
				{
					photonEmittance( hit, photon, ray );
					return;
				}
					
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
				vec3 dir = k < 0 ? 0.0f : etaiOverEtat * ray.direction + ( etaiOverEtat * cosi - sqrtf( k ) ) * n;
				
				
				
				if ( reflectChance > refractChance )
				{
					Ray reflectRay( hit.point, reflect( ray.direction, hit.normal ), INFINITY, ray.depth + 1 );
					return SampleP( reflectRay, hit, photon );
				}
				else
				{
					point3 refractRayOrigin = hit.isFrontFace ? hit.point - hit.normal * 0.001 : hit.point + hit.normal * 0.001f;

					photon.position = hit.point;
					photon.power = mCol;	 
					photon.L = ray.direction; 
					push( causticmap, photon );

					return SampleP( Ray( refractRayOrigin, dir, INFINITY, ray.depth + 1.0 ), hit, photon );
				}
			}
			if (mmat == MaterialType::MIRROR)
			{
				Ray r( hit.point, reflect( ray.direction, hit.normal ) + ( 1.0f - hit.material->smoothness ) * RandomInsideUnitSphere(), INFINITY, ray.depth + 1 ); //new ray from intersection point
			
				SampleP( r, hit, photon ) ;		
				return;
			}
	}
	return;
}

//TODO: sort (and add) the kD-tree
/*
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



// -- PHASE 2 --

//photonmaptracer
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
	vec3 unit_direction = ray.direction;
	auto t = 0.5 * ( -unit_direction.y + 1.0 );
	color c = ( 1.0 - t ) * color( 1.0, 1.0, 1.0 ) + t * color( 0.5, 0.7, 1.0 );
	return c;
}


 color PhotonMap::photonDensity( Ray &ray, RayHit hit, color BRDF )
{
	color energy = (0,0,0);  
	int countP = 0;
	int countC = 0;
	for (int i = 0; i < topStackP; i++)
	{
		float insideR = sqrt(pow( ( photonmap[i].position.x - hit.point.x ), 2 ) + pow( ( photonmap[i].position.y - hit.point.y ), 2 ) + pow( ( photonmap[i].position.z - hit.point.z ), 2 ));
		
		//inside 'density sphere'
		if (insideR < estimateR)
		{
			float weight = max( 0.0f, -dot( hit.normal, photonmap[i].L ) );
			//weight *= ( 1.0 - insideR );

			//add to energy
			energy += photonmap[i].power *weight;
			countP++;
		}
	}

	energy = energy / max(1, countP);

	color causticEnergy( 0.0 );
	for (int i = 0; i < topStackC; i++)
	{
		float insideR = sqrt(pow( ( causticmap[i].position.x - hit.point.x ), 2 ) + pow( ( causticmap[i].position.y - hit.point.y ), 2 ) + pow( ( causticmap[i].position.z - hit.point.z ), 2 ));

		//inside 'density sphere'
		if ( insideR <  estimateRC )
		{
			float weight = max( 0.0f, -dot( hit.normal, photonmap[i].L ) );
			//weight *= ( 1.0 -  sqrt(insideR)  );

			//add to energy
			causticEnergy += causticmap[i].power * weight;
			countC++;
		}
	}
	causticEnergy = causticEnergy / max( 1, countC );

	return (BRDF * energy);
}

