/** \file material.cc
    \brief Implements all methods for all materials (SimpleMaterial,
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

#include "general.h"
#include "material.h"
#include "noise.h"

Material::Material() :ReferencedObject() {}

SimpleMaterial::SimpleMaterial(LightingProperties *props) :Material() { properties = *props; }
void SimpleMaterial::getLightingProperties(double point[3],LightingProperties *props,double normal[3]) {
  *props = properties;
}

CheckerboardMaterial::CheckerboardMaterial(double size,LightingProperties *propsA,LightingProperties *propsB) 
  :Material()
{ 
  this->size=size;
  properties[0] = *propsA; 
  properties[1] = *propsB; 
}
void CheckerboardMaterial::getLightingProperties(double point[3],LightingProperties *props,double normal[3]) {
  *props = properties[((int)(100000+point[0]/size) + (int)(100000+point[1]/size) + (int)(100000+point[2]/size)) % 2];
}

NoiseMaterial::NoiseMaterial() :Material() {}
void NoiseMaterial::getLightingProperties(double point[3],LightingProperties *props,double normal[3]) {
  double scale=1.0;
  double amplitude=1.0;
  double offset[3];
  int i;
  double v=0.0;
  zero(offset);
  for(i=0;i<6;i++) {
    v+=amplitude*noise(point[0]*scale+offset[0],point[1]*scale+offset[1],point[2]*scale+offset[2]);
    scale *= 2.0;
    amplitude *= 0.5;
  }
  v=0.5+0.3*v;
  
  if(v < 0.0) v = 0.0;
  else if(v > 1.0) v = 1.0;
  zero(props->ambient);
  for(i=0;i<3;i++) props->diffuse[i]=v;
  zero(props->specular);
  zero(props->reflection);
  props->shininess=10.0;
}

MaterialMap::MaterialMap(Function fun) 
  :Material()
{
  function = fun;
  nNodes=0;
}
void MaterialMap::add(double nodePosition,LightingProperties*nodeProperty) {
  int insertionPoint;
  int i;
  /* First, find correct place in list of nodes to insert this node in */
  for(insertionPoint=0;insertionPoint<nNodes;insertionPoint++)
    if(nodePositions[insertionPoint] > nodePosition) break;
  /* Move all other existing nodes forward one step to make place
     for this one */
  for(i=nNodes;i>insertionPoint;i--) { 
    nodePositions[i]=nodePositions[i-1];
    nodeProperties[i]=nodeProperties[i-1];
  }
  nNodes++;
  /* Insert this node into the proper place */
  nodePositions[insertionPoint]=nodePosition;
  nodeProperties[insertionPoint]=*nodeProperty; 
}
void MaterialMap::getLightingProperties(double point[3],LightingProperties *props,double normal[3]) {
  int i;
  double val;

  double scale=1.0;
  double amplitude=1.0;
  double offset[3];
  val=0.0;
  zero(offset);
  for(i=0;i<6;i++) {
    val+=amplitude*noise(point[0]*scale+offset[0],point[1]*scale+offset[1],point[2]*scale+offset[2]);
    scale *= 2.0;
    amplitude *= 0.5;
  }  

  int highNode;
  int lowNode;
  double fracL;
  double fracH;
  /* TODO - do this search in a smarter way, eg. start at nNodes/2 and
     with a step size of nNodes/4 etc. */
  for(highNode=0,lowNode=0;highNode<nNodes;highNode++,lowNode=highNode-1)
    if(nodePositions[highNode] > val) break;
  if(highNode == nNodes) { *props = nodeProperties[nNodes-1]; }
  else if(lowNode == highNode) { *props = nodeProperties[0]; }
  else {
    double delta=nodePositions[highNode]-nodePositions[lowNode];
    if(delta < 1e-5) *props = nodeProperties[lowNode];
    else {
      fracH = (val - nodePositions[lowNode])/delta;
      fracL = 1.0 - fracH;
      LightingProperties *low=&nodeProperties[lowNode],*high=&nodeProperties[highNode];
      props->ambient[0] = fracH*high->ambient[0] + fracL*low->ambient[0];
      props->ambient[1] = fracH*high->ambient[1] + fracL*low->ambient[1];
      props->ambient[2] = fracH*high->ambient[2] + fracL*low->ambient[2];
      props->diffuse[0] = fracH*high->diffuse[0] + fracL*low->diffuse[0];
      props->diffuse[1] = fracH*high->diffuse[1] + fracL*low->diffuse[1];
      props->diffuse[2] = fracH*high->diffuse[2] + fracL*low->diffuse[2];
      props->specular[0] = fracH*high->specular[0] + fracL*low->specular[0];
      props->specular[1] = fracH*high->specular[1] + fracL*low->specular[1];
      props->specular[2] = fracH*high->specular[2] + fracL*low->specular[2];
      props->shininess = fracH*high->shininess + fracL*low->shininess;
      props->reflection[0] = fracH*high->reflection[0] + fracL*low->reflection[0];
      props->reflection[1] = fracH*high->reflection[1] + fracL*low->reflection[1];
      props->reflection[2] = fracH*high->reflection[2] + fracL*low->reflection[2];
    }
  }
}
