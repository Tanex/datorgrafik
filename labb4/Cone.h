#pragma once

#include "object.h"

class Cone : public Object
{
public:
	double lineTest(double origin[3], double direction[3], double maxDistance);
	void getNormal(double point[3], double normal[3]);
	bool isInside(double point[3]);
};
