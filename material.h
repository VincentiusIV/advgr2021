#pragma once

enum class MaterialType
{
	DIFFUSE = 0,
	MIRROR = 1,
	GLASS = 2,
	DIELECTRIC = 3,
	NORMAL_TEST = 4,
};

class Material
{
  public:
	Material( Color color, MaterialType materialType ) : color(color), materialType(materialType) {}

	Color color;
	float n = 1.0f, specularity = 1.0f, smoothness = 0.5f;
	MaterialType materialType;
};
