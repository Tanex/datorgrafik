/** \file plane.cc
    \brief Implements all methods for the Plane class.
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
#include "plane.h"

Plane::Plane(double normal[3],double offset) {
  assign(normal,this->normal);
  this->offset = offset;
}
Plane::~Plane() {}

double Plane::lineTest(double O[3],double D[3],double maxDistance) {
  double alpha = -(dotProduct(normal,O) - offset) / dotProduct(normal,D);

  /*  This is just to illustrate how you can debug your linetest functions */
  if(debugThisPixel) {
    printDebugIndentation(); 
    printf("-> Plane::lineTest (%3.2f,%3.2f,%3.2f)+a(%3.2f,%3.2f,%3.2f)\n",
	   O[0],O[1],O[2],D[0],D[1],D[2]);
    printDebugIndentation(); printf("<- Dist %.2f\n",(alpha>0&&alpha<maxDistance)?alpha:MAX_DISTANCE);
  }

  if(alpha > 0 && alpha < maxDistance) return alpha;
  else return MAX_DISTANCE;
}

void Plane::getNormal(double point[3],double normal[3]) {
  assign(this->normal,normal);
}

bool Plane::isInside(double point[3]) {
  return +dotProduct(point,normal) < offset;
}
 
