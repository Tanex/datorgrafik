/** \file raytracer.cc
    \brief Implements the main raytracing functionality for the
    Raytracer class. 
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
#include "raytracer.h"

using namespace std;

extern int screenWidth, screenHeight;

Raytracer::Raytracer() { 
  zero(background);
  zero(ambientLight);
  lights = new set<Light*>();
  objects = new set<Object*>();
}
Raytracer::~Raytracer() {
  set<Object*>::iterator objIterator;
  set<Object*>::iterator objIteratorEnd;
  for(objIterator=objects->begin(),objIteratorEnd=objects->end();
      objIterator != objIteratorEnd;objIterator++) {
    (*objIterator)->dereference();
  }
  set<Light*>::iterator lightIterator;
  set<Light*>::iterator lightIteratorEnd;
  for(lightIterator=lights->begin(),lightIteratorEnd=lights->end();
      lightIterator != lightIteratorEnd;lightIterator++) {
    (*lightIterator)->dereference();
  }
}
void Raytracer::setBackground(double col[3]) { assign(col,background); }
void Raytracer::setAmbientLight(double col[3]) { assign(col,ambientLight); }

void Raytracer::addObject(Object *object) { 
  object->reference(); 
  objects->insert(object); 
}
void Raytracer::addLight(Light *light) { 
  light->reference(); 
  lights->insert(light); 
}
void Raytracer::setCamera(Camera *cam) { camera = cam; }
Camera *Raytracer::getCamera() { return camera; }
void Raytracer::raytrace(int x,int y,double rgb[3]) {
  double origin[3], direction[3];
  camera->getPixelRay(x/(double)screenWidth,y/(double)screenHeight,origin,direction);
  raytrace(origin,direction,rgb,1.0);
}
void Raytracer::raytrace(double origin[3], double direction[3], double rgb[3],double contribution) {
  int i;
  double closestDistance, distance;
  Object *closestObject;
  set<Object*>::iterator objIterator;
  set<Object*>::iterator objIteratorEnd;
  set<Light*>::iterator lightIterator;
  set<Light*>::iterator lightIteratorEnd;

  if(debugThisPixel) {
    printDebugIndentation(); debugIndentation++; printf("-> Raytrace\n");
  }

  /* Iterate over all objects and find the closest one that
     intersects this ray. */
  for(closestDistance=MAX_DISTANCE,closestObject=NULL,
	objIterator=objects->begin(),objIteratorEnd=objects->end();
      objIterator != objIteratorEnd;objIterator++) {
    Object *object = *objIterator;
    distance = object->lineTest(origin,direction,closestDistance);
    if(distance < closestDistance && distance > 1e-5) {
      closestDistance = distance;
      closestObject = object;
    }
  }

  if(closestDistance >= MAX_DISTANCE) {
    /* No objects hit, assign background colour to ray instead. */
    for(i=0;i<3;i++) rgb[i] = background[i];
    if(debugThisPixel) { debugIndentation--; printDebugIndentation(); printf("<- Miss\n"); }
    return;
  }
   
  double point[3], normal[3];
  for(i=0;i<3;i++) point[i] = origin[i]+closestDistance*direction[i];
  closestObject->getNormal(point,normal);

  /* Get lighting properties for this point */
  LightingProperties properties;
  closestObject->getLightingProperties(point,&properties,normal);
  normalize(normal);

  double E[3]; /* Vector towards eye. */
  for(i=0;i<3;i++) E[i] = -direction[i];
  normalize(E);

  /* Now, compute the colour for this point */
  /* Ambient light first */
  for(i=0;i<3;i++) rgb[i] = properties.ambient[i] * ambientLight[i];

  /* Iterate over all the lights and add their colours to the rgb
     colour using the Blinn-Phong shading model. */
  for(lightIterator=lights->begin(),lightIteratorEnd=lights->end();
      lightIterator!=lightIteratorEnd;lightIterator++) {
    Light *light = *lightIterator;
    double L[3]; /* Light vector */
    sub(light->position,point,L); 
    double lightDistance=length(L);
    for(i=0;i<3;i++) L[i]=L[i]/lightDistance;
    /* First, cast a shadow feeler */
    for(objIterator=objects->begin(),objIteratorEnd=objects->end();
	objIterator != objIteratorEnd;objIterator++) {
      Object *object = *objIterator;
      if(object == closestObject) 
	/* For now, ignore shadows cast on ourselves */
	continue; 
      distance = object->lineTest(point,L,lightDistance);
      if(distance < lightDistance) break;
    }
    if(objIterator != objIteratorEnd)
      /* A shadow was found, so ignore this light */
      continue; 

    /*printf("L: %3.1f %3.1f %3.1f\n",L[0],L[1],L[2]);*/
    double diffusePower = dotProduct(normal,L);
    if(diffusePower > 0) {
      /* Light is shining on the front of the object. */
      for(i=0;i<3;i++) rgb[i] += diffusePower * light->colour[i] * properties.diffuse[i];

      double RL[3]; /* Reflection of light vector */
      double LdotN = dotProduct(L,normal);
      for(i=0;i<3;i++) RL[i] = 2.0 * LdotN * normal[i] - L[i];

      double specDot=dotProduct(RL,E);
      if(specDot > 0.0) {
	double specularPower = pow(specDot, properties.shininess);
	for(i=0;i<3;i++) {
	  rgb[i] += specularPower * properties.specular[i];
	}
      }     
    } else {
      /* Light is shining from behind object... ignore for now. To be
	 slightly more correct we should also allow the light to shine
	 through the object and multiply it by the transparent colour
	 of the object. */
    }    
  }

  /* Add light that is reflected on object */
  double reflection = 0.4*properties.reflection[0]+0.4*properties.reflection[1]+0.2*properties.reflection[2];
  if(contribution*reflection > 0.05) {
    double rgbTmp[3];   /* Temporary colour for incoming light */
    double R[3];        /* Reflection vector */
    double s=2.0*dotProduct(E,normal);
    double pointR[3];   /* From from where to trace */
    for(i=0;i<3;i++) {
      R[i]=s*normal[i] - E[i];
      pointR[i]=point[i]+R[i]*1e-4;  /* To void numberical instability */
    }
    /* Recurse on this ray to get incoming light level */
    raytrace(point,R,rgbTmp,contribution*reflection);
    /* Add the incomming light to the colour of this pixel */
    for(i=0;i<3;i++) rgb[i] += rgbTmp[i]*properties.reflection[i];
  }

  // Check for transparency
  if (properties.transparency > 0)
  {
	  double rgbTrans[3];
  }

  if(debugThisPixel) { 
    debugIndentation--; printDebugIndentation(); printf("<- RGB %.1f %.1f %.1f\n",rgb[0],rgb[1],rgb[2]);
  }
}
