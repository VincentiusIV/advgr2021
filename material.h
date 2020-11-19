#pragma once

enum class MaterialType
{
	DIFFUSE = 0,
	MIRROR = 1,
	GLASS = 2,
	NORMAL_TEST = 3,
};

class Material
{
  public:
	Material( Color color, MaterialType materialType ) : color(color), materialType(materialType) {}

	Color color;
	MaterialType materialType;
};
