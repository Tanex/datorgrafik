/** \file material.h
    \brief Declares all methods for all materials (SimpleMaterial,
    CheckerboardMaterial, NoiseMaterial, MaterialMap). 
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

#ifndef   	MATERIAL_H_
# define   	MATERIAL_H_

#ifndef REFERENCED_H
#include "referenced.h"
#endif

/** \brief Stores lighting properties used for Phong lighting,
    reflections and transparancy. 

    Must be given by any custom defined material on a per pixel
    basis. */
class LightingProperties {
 public:
  double ambient[3];
  double diffuse[3];
  double specular[3], shininess;
  double reflection[3];
}; 

/** \brief Abstract base class for all materials.

    These objects are used to compute the lighting properties for any
    specific point in the scene using the virtual function
    Material::getLightingProperties().
*/
class Material : public ReferencedObject {
 public:
  Material();

  /** Returns the lighting properites at a certain point. Is also
      given the current normal at that point, and can modify it. */
  virtual void getLightingProperties(double point[3],LightingProperties *props,double normal[3])=0;
 private:
};

/** \brief Implements a simple material with the same properties over
    all of the whole object. */
class SimpleMaterial : public Material {
 public:
  SimpleMaterial(LightingProperties*);

  virtual void getLightingProperties(double point[3],LightingProperties *props,double normal[3]);
 private:
  LightingProperties properties;
}; 

/** \brief A checkerboard pattern material.

    Requires checkerboard size and two sets of properties. */
class CheckerboardMaterial : public Material {
 public:
  CheckerboardMaterial(double size,LightingProperties*,LightingProperties*);

  virtual void getLightingProperties(double point[3],LightingProperties *props,double normal[3]);
 private:
  double size;
  LightingProperties properties[2];
};

/** \brief Creates gray noise all over the object */
class NoiseMaterial : public Material {
 public: 
  NoiseMaterial();
  virtual void getLightingProperties(double point[3],LightingProperties *props,double normal[3]);
 private:
};

#define MAX_MATERIAL_MAP_NODES 16
/** \brief Generic function mapped material.

    Interpolatates between multiple lighting properties depending on a
    given function. To use, instantiate with a given function
    and add all LightingProperties nodes that you want to interpolate
    between. Each node should be given a function value for which it will
    contribute 100% to the resulting material property. */
class MaterialMap : public Material {
 public:
  typedef enum { Noise } Function;
  MaterialMap(Function fun);
  void add(double nodePosition,LightingProperties*nodeProperties);
  virtual void getLightingProperties(double point[3],LightingProperties *props,double normal[3]);
 private:
  Function function;
  double nodePositions[MAX_MATERIAL_MAP_NODES];
  LightingProperties nodeProperties[MAX_MATERIAL_MAP_NODES];
  int nNodes;
};

#endif 	    /* !MATERIAL_H_ */
