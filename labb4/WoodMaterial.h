#pragma once

#include "material.h"

class WoodMaterial : public Material {
public:
	WoodMaterial();

	virtual void getLightingProperties(double point[3], LightingProperties *props, double normal[3]);
private:
	LightingProperties cDark;
	LightingProperties cLight;
};
