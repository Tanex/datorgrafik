/** \file object.cc
    \brief Implements all generic methods for all raytracable
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

#include "general.h"
#include "material.h"
#include "object.h"

Object::Object() :Material() { material = NULL; }
Object::~Object() {if(material) material->dereference(); }
void Object::setMaterial(Material *mat) { 
  if(material) material->dereference(); 
  material = mat; 
  material->reference(); 
}
void Object::getLightingProperties(double point[3],LightingProperties *props,double normal[3]) {
  if(material)
    material->getLightingProperties(point,props,normal);
}
