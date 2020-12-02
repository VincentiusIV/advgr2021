#pragma once

enum class MaterialType
{
	DIFFUSE = 0,
	MIRROR = 1,
	GLASS = 2,
	DIELECTRIC = 3,
	EMISSIVE = 4,
	NORMAL_TEST = 11,
	UV_TEST = 12
};

class Material
{
  public:
	Material( color color, MaterialType materialType ) : albedo( color ), materialType( materialType ), uvScale( vec2( 1.0, 1.0 ) ) {}

	color GetColor( vec2 uv ) 
	{ 
		if ( mainTex != NULL )
		{
			int u = uv.x * ( mainTex->GetWidth() - 1 );
			int v = uv.y * ( mainTex->GetHeight() - 1 );
			uint intColor = mainTex->GetBuffer()[v * mainTex->GetHeight() + u];
			const unsigned int r = ( intColor & REDMASK ) >> 16;
			const unsigned int g = ( intColor & GREENMASK ) >> 8;
			const unsigned int b = ( intColor & BLUEMASK );
			return color( r / 255.999, g / 255.999, b / 255.999 );
		}
		else
		{
			return albedo; 		
		}
	}
	color albedo;
	float n = 1.0f, specularity = 1.0f, smoothness = 1.0f;
	vec2 uvScale;
	MaterialType materialType;
	Surface* mainTex = NULL;
	bool isCheckerboard = false;
};
