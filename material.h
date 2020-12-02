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
	Material( color color, MaterialType materialType ) : color( color ), materialType( materialType ), uvScale( vec2( 1.0, 1.0 ) ) {}

	color GetColor( vec2 uv ) { return color; }
	color color;
	float n = 1.0f, specularity = 1.0f, smoothness = 1.0f;
	vec2 uvScale;
	MaterialType materialType;
	Surface mainTex;
	bool isCheckerboard;
};
