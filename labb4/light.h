/** \file light.h
    \brief Declares all methods for the Light class.
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

#ifndef   	LIGHT_H_
# define   	LIGHT_H_

#ifndef REFERENCED_H
#include "referenced.h"
#endif

/** \brief Represents a simple direct point light in the scene. 
    
    Must be added to the raytracer using the raytracer::addLight function. */
class Light :public ReferencedObject {
 public:
  Light(double position[3],double colour[3]);
 private:
  double position[3];
  double colour[3];
  
  friend class Raytracer;
};

#endif 	    /* !LIGHT_H_ */
