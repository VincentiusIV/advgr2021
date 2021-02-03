#pragma once

enum class MaterialType
{
	DIFFUSE = 0,
	MIRROR = 1,
	GLASS = 2,
	DIELECTRIC = 3,
	EMISSIVE = 4,
	VOLUMETRIC = 5,
	NORMAL_TEST = 11,
	UV_TEST = 12
};

class Material
{
  public:
	Material( color color, MaterialType materialType ) : albedo( color ), materialType( materialType ), uvScale( vec2( 1.0, 1.0 ) ) {}

	color GetColor( vec2 uv ) 
	{
		if ( isCheckerboard )
		{
			int u = fmin( uv.x, 1.0 ) * 2;
			int v = fmin( uv.y, 1.0 ) * 2;
			if ( u % 2 == 1 && v % 2 == 1 || u % 2 == 0 && v % 2 == 0)
				return albedo;
			else
				return color( 1, 1, 1 );
		}
		else if ( mainTex != NULL && mainTex->GetWidth() >0)
		{
			int u = fmin(uv.x, 1.0) * ( mainTex->GetWidth() - 1 );
			int v = fmin(uv.y, 1.0 ) * ( mainTex->GetHeight() - 1 );
			uint intColor = mainTex->GetBuffer()[v * mainTex->GetHeight() + u];
			const unsigned int r = ( intColor & REDMASK ) >> 16;
			const unsigned int g = ( intColor & GREENMASK ) >> 8;
			const unsigned int b = ( intColor & BLUEMASK );
			return color( r / 255.999, g / 255.999, b / 255.999 ) * albedo;
		}
		else
		{
			return albedo; 		
		}
	}

	float sigmaT() { return sigmaS + sigmaA; }
	float volumeAlbedo() { return sigmaS / sigmaT(); } // describes probability of scattering at scatter event.
	float meanFreePath() { return 1 / sigmaT(); } // avg distance a ray travels in medium before interacting with particle.

	color albedo, emission;
	float n = 1.0f, specularity = 1.0f, smoothness = 1.0f;

	float sigmaS = 0.5, sigmaA = 0.33, g = 0.5;

	vec2 uvScale;
	MaterialType materialType;
	Surface* mainTex = NULL;
	bool isCheckerboard = false;
};
