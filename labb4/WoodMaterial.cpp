#include "WoodMaterial.h"
#include "noise.h"
#include <cmath>

static const double C = 50.0;

static const double DARK_AMBIENT[3] = { 0.3, 0.2, 0.0 };
static const double DARK_DIFFUSE[3] = { 0.3, 0.2, 0.0 };
static const double DARK_SPECULAR[3] = { 0.3, 0.2, 0.0 };
static const double DARK_REFLECTION[3] = { 0.0, 0.0, 0.0 };
static const double DARK_SHININESS = 100.0;

static const double LIGHT_AMBIENT[3] = { 0.7, 0.5, 0.0 };
static const double LIGHT_DIFFUSE[3] = { 0.7, 0.5, 0.0 };
static const double LIGHT_SPECULAR[3] = { 0.7, 0.5, 0.0 };
static const double LIGHT_REFLECTION[3] = { 0.0, 0.0, 0.0 };
static const double LIGHT_SHININESS = 1000.0;

WoodMaterial::WoodMaterial() :Material()
{
	for (int i = 0; i < 3; i++)
	{
		cDark.ambient[i] = DARK_AMBIENT[i];
		cDark.diffuse[i] = DARK_DIFFUSE[i];
		cDark.specular[i] = DARK_SPECULAR[i];
		cDark.reflection[i] = DARK_REFLECTION[i];	

		cLight.ambient[i] = LIGHT_AMBIENT[i];
		cLight.diffuse[i] = LIGHT_DIFFUSE[i];
		cLight.specular[i] = LIGHT_SPECULAR[i];
		cLight.reflection[i] = LIGHT_REFLECTION[i];
	}

	cDark.shininess = DARK_SHININESS;
	cLight.shininess = LIGHT_SHININESS;
}

void WoodMaterial::getLightingProperties(double point[3], LightingProperties *props, double normal[3]) 
{
	double r = sqrt(pow(point[0], 2) + pow(point[1], 2))*noise(point[1]);
	double b = fmod(r * (C/(1+abs(r))), 1.0);

	for (int i = 0; i < 3; i++)
	{
		props->ambient[i] = (1 - b)*cDark.ambient[i] + b*cLight.ambient[i];
		props->diffuse[i] = (1 - b)*cDark.diffuse[i] + b*cLight.diffuse[i];
		props->specular[i] = (1 - b)*cDark.specular[i] + b*cLight.specular[i];
		props->reflection[i] = (1 - b)*cDark.reflection[i] + b*cLight.reflection[i];
	}
	props->shininess = (1 - b)*cDark.shininess + b*cLight.shininess;
}