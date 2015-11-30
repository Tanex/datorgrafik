/** \file transform.h
    \brief Declares all methods for the Transform class.
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

#ifndef   	TRANSFORM_H_
# define   	TRANSFORM_H_

#ifndef OBJECT_H_
#include "object.h"
#endif

/** \brief Represents generic affine transformations. 
    
    Can contain a single child object and performs a transformation
    on it's position and shape. */
class Transform : public Object {
 public:
  Transform(Object *child);
  ~Transform();

  double lineTest(double origin[3],double direction[3],double maxDistance);
  void getNormal(double point[3],double normal[3]);
  bool isInside(double point[3]);
  
  /** Resets the transform to the identity matrix */
  void identity();

  /** Performs a translation */
  void translate(double dx,double dy,double dz);

  /** Performs a scale */
  void scale(double sx,double sy,double sz);

  /** Rotates around X-axis */
  void rotateX(double rad);

  /** Rotates around Y-axis */
  void rotateY(double rad);

  /** Rotates around Z-axis */
  void rotateZ(double rad);

  virtual void getLightingProperties(double point[3],LightingProperties *props,double normal[3]);
 private:
  void computeInverseTransform();

  Matrix4d forward;
  Matrix4d inverse;
  Object *child;
};

#endif 	    /* !TRANSFORMS_H_ */
