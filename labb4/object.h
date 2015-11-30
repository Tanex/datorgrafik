/** \file object.h
    \brief Declares all generic methods for all raytracable
    objects. 
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

#ifndef   	OBJECT_H_
# define   	OBJECT_H_

#ifndef MATERIAL_H_
#include "material.h"
#endif

#define MAX_DISTANCE 1e9

/** \brief Abstract base class for all renderable objects. 

    Each concrete instance of this class must contain virtual
    functions that are used to perform line tests, computing normals
    and lighting properties. */
class Object : public Material {
 public:
  Object();
  /** All objects must have a virtual destructor so that they can be
      deallocated properly and to dereference any contained objects. */
  virtual ~Object()=0;
  
  /** Must do the line intersection test and return the closest distance
  in which a ray originating in origin with given direction intersects
  the object - if such a point exists closer than maxDistance. Returns
  distance MAX_DISTANCE if no such intersection exists. 

  Direction is not neccessarily a vector of unit length, and distances
  are measured in multiples of this vector. */
  virtual double lineTest(double origin[3],double direction[3],double maxDistance)=0;

  /** Compute the normal of the object at the given point. Result is
      not guaranteed to be of unit length. */
  virtual void getNormal(double point[3],double normal[3])=0;

  /** Tests if a point is inside the object or not. */
  virtual bool isInside(double point[3])=0;

  /** Sets up a default material to use for this object. */
  virtual void setMaterial(Material *material);

  /** Default material handling that queries the default material
      given for this object. */
  virtual void getLightingProperties(double point[3],LightingProperties *props,double normal[3]);

 protected:
  /** Material used by the default getLightingProperties function. */
  Material *material;
};

#endif 	    /* !OBJECT_H_ */
