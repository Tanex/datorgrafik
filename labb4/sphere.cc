/** \file sphere.cc
    \brief Implements all methods for the Sphere class.
*/
/* Made by Mathias Broxvall 

   Copyright (C) 2007  Mathias Broxvall

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#include "general.h"
#include "sphere.h"

Sphere::Sphere(double radius) {
  this->radius = radius;
}
Sphere::~Sphere() {}

double Sphere::lineTest(double O[3],double dir[3],double maxDistance) {
  /* Solving this lineIntersection test is equal to solving the second
     degree formula "a X^2 + b X + C = 0" for a, b, c given below. */
  //printf("ray: (%3.2f %3.2f %3.2f) + alpha * (%3.2f %3.2f %3.2f)\n",O[0],O[1],O[2],dir[0],dir[1],dir[2]);
  double a = dotProduct(dir,dir);
  double b = 2 * dotProduct(O,dir);
  double c = dotProduct(O,O) - radius*radius;
  double s = b * b - 4*a*c;
  if(s < 0) { return MAX_DISTANCE; }  
  s = sqrt(s);
  double sol1 = (-b - s)/(2*a);
  if(sol1 > 0) return sol1;
  double sol2 = (-b + s)/(2*a);
  if(sol2 > 0)  return sol2;
  return MAX_DISTANCE;
}

void Sphere::getNormal(double point[3],double normal[3]) {
  assign(point,normal);
}

bool Sphere::isInside(double point[3]) {
  return dotProduct(point,point) < radius*radius;
}
