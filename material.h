#pragma once

enum class MaterialType
{
	DIFFUSE = 0,
	MIRROR = 1,
	GLASS = 2,
	DIELECTRIC = 3,
	NORMAL_TEST = 4,
	EMISSIVE = 5
};

class Material
{
  public:
	Material( color color, MaterialType materialType ) : color(color), materialType(materialType) {}

	color color;
	float n = 1.0f, specularity = 1.0f, smoothness = 1.0f;
	MaterialType materialType;
};
