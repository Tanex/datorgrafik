/** \file sphere.h
    \brief Declares all methods for the Sphere class.
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

#ifndef   	SPHERE_H_
# define   	SPHERE_H_

#ifndef OBJECT_H_
#include "object.h"
#endif

/** \brief A sphere of given radius.         
 */
class Sphere : public Object {
 public:
  Sphere(double radius);
  ~Sphere();

  double lineTest(double origin[3],double direction[3],double maxDistance);
  void getNormal(double point[3],double normal[3]);
  bool isInside(double point[3]);

 private:
  double radius;
};

#endif 	    /* !SPHERE_H_ */
