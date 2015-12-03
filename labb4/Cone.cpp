#include "Cone.h"
#include <cmath>

double Cone::lineTest(double origin[3], double direction[3], double maxDistance)
{
	double a = direction[0] * direction[0] + direction[1] * direction[1] - direction[2] * direction[2];
	double b = 2 * origin[0] * direction[0] + 2 * origin[1] * direction[1] - 2 * origin[2] * direction[2];
	double c = origin[0] * origin[0] + origin[1] * origin[1] - origin[2] * origin[2];

	double s = b * b - 4 * a*c;
	//Negative number in root expression
	if (s < 0) 
		return MAX_DISTANCE;

	s = sqrt(s);
	double sol1 = (-b - s) / (2 * a);
	if (sol1 > 0 && (origin[2] + direction[2]*sol1) >= 0) 
		return sol1;
	double sol2 = (-b + s) / (2 * a);
	if (sol2 > 0 && (origin[2] + direction[2] * sol2) >= 0) 
		return sol2;
	return MAX_DISTANCE;
}

void Cone::getNormal(double point[3], double normal[3])
{
	normal[0] = 2 * point[0];
	normal[1] = 2 * point[1];
	normal[2] = -2 * point[2];
}

bool Cone::isInside(double point[3])
{
	return ((point[0] * point[0] + point[1] * point[1] - point[2] * point[2]) <= 0) && point[2] >= 0;
}