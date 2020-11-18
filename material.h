#pragma once

enum MaterialType
{
	DIFFUSE,
	MIRROR,
	GLASS
};

class Material
{
  public:
	Material( Color color, MaterialType materialType ) : color(color), materialType(materialType) {}

	Color color;
	MaterialType materialType;
};
