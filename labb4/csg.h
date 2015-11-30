/** \file csg.h
    \brief Declares all methods for all CSG classes (Intersection, Inverse)
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

#ifndef   	CSG_H_
# define   	CSG_H_

#ifndef OBJECT_H_
#include "object.h"
#endif

/** \brief Creates new objects as the intersection of multiple child
    objects. 

    This is one of the basic CSG building blocks of more complex
    geometry. In this current implementation it is done in a generic,
    but inefficeint manner. More efficient code can be made using
    harder restrictions on the contained child objects.  */
class Intersection : public Object {
 public:
  Intersection();
  ~Intersection();
  void addObject(Object *);
  
  double lineTest(double origin[3],double direction[3],double maxDistance);
  void getNormal(double point[3],double normal[3]);
  bool isInside(double point[3]);

  void getLightingProperties(double point[3],LightingProperties *props,double normal[3]);

 private:
  class std::set<Object*> *objects;
  /* This will hold a pointer to the last used object, saved on a per thread basis */
  Object *lastObject[MAX_OMP_THREADS];
};

/** \brief Creates the inverse of an object by negating the
    Object::isInside and Object::GetNormal functions. */
class Inverse : public Object {
 public:
  Inverse(Object *);
  ~Inverse();

  double lineTest(double origin[3],double direction[3],double maxDistance);
  void getNormal(double point[3],double normal[3]);
  bool isInside(double point[3]);

  void getLightingProperties(double point[3],LightingProperties *props,double normal[3]);
 private:
  Object *object;
};

#endif 	    /* !CSG_H_ */
