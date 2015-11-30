/** \file csg.cc
    \brief Implements all methods for all CSG classes (Intersection, Inverse)
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
#include "csg.h"
#include <omp.h>

using namespace std;

Intersection::Intersection() {
  objects = new std::set<Object*>();
  for(int i=0;i<MAX_OMP_THREADS;i++) lastObject[i] = NULL;
}
Intersection::~Intersection() {
  set<Object*>::iterator objIterator;
  set<Object*>::iterator objIteratorEnd;
  for(objIterator=objects->begin(),objIteratorEnd=objects->end();
      objIterator != objIteratorEnd;objIterator++) {
    (*objIterator)->dereference();
  }
}
void Intersection::addObject(Object *object) {
  object->reference();
  objects->insert(object);
}

double Intersection::lineTest(double O1[3],double dir[3],double maxDistance) {
  int i;
  bool findOutsides;
  double O[3],point[3];
  double dist, progress, offset;
  set<Object*>::iterator objIterator;
  set<Object*>::iterator objIteratorEnd;

  /*  This is just to illustrate how you can debug your linetest functions */
  if(debugThisPixel) {
    printDebugIndentation(); 
    printf("-> Intersection::lineTest\n");
    debugIndentation++;
  }

  findOutsides = isInside(O1);
  assign(O1,O);
  offset=0.0;
  while(1) {
    progress = maxDistance;
    dist = maxDistance;

    for(objIterator=objects->begin(),objIteratorEnd=objects->end();
	objIterator != objIteratorEnd;objIterator++) {
      double thisDist = (*objIterator)->lineTest(O,dir,maxDistance);
      if(thisDist >= maxDistance) continue;
      if(thisDist < progress) progress = thisDist + 1e-5;
      for(i=0;i<3;i++) point[i] = O[i]+(thisDist+1e-3)*dir[i];
      if(thisDist < dist && (isInside(point) ^ findOutsides)) {
	dist = thisDist;
	lastObject[omp_get_thread_num()] = *objIterator;
      }
    }  
    if(dist < maxDistance) 
      /* Found closest point inside all objects */
      break; 
    else if(progress >= maxDistance) {
      /* no intersection with any points found */
      if(debugThisPixel) { debugIndentation--; printDebugIndentation(); printf("<- miss (1)\n"); }      
      return MAX_DISTANCE;
    }
    else {
      /* If no line intersection that satisfied the inside test was
      found, we have to try again from a new vantage point to find the
      next set of solutions for each object, if it exists. */
      if(progress < 1e-5) {
	printf("ERROR\n"); exit(0);
      }
      for(i=0;i<3;i++) O[i] += progress*dir[i];      
      offset += progress;
      maxDistance -= progress;
      if(maxDistance < 0.0) 
	/* We have progresses too far ahead, intersections no longer
	   interesting */
	if(debugThisPixel) { debugIndentation--; printDebugIndentation(); printf("<- miss (2)\n"); }
	return MAX_DISTANCE;
    }
  }

  if(debugThisPixel) {
    debugIndentation--; printDebugIndentation(); 
    printf("<- %3.2f\n",dist+offset);
  }

  return dist + offset;
}

void Intersection::getNormal(double point[3],double normal[3]) {
  /* Simplifying assumption, this function is only called after a
  successfull lineintersection test. Otherwise we would have to
  compute which object is actually the limiting object at this point.
  This is a lot faster than the alternative to compute the correct
  intersecting point, but it is not correct when running with multiple
  threads and not even with a single thread and some special scene
  graphs. */
  lastObject[omp_get_thread_num()]->getNormal(point,normal);
}

bool Intersection::isInside(double point[3]) {
  /* Iterate over all objects, return true if we are inside all of
     them */
  set<Object*>::iterator objIterator;
  set<Object*>::iterator objIteratorEnd;
  for(objIterator=objects->begin(),objIteratorEnd=objects->end();
      objIterator != objIteratorEnd;objIterator++) {
    if(!(*objIterator)->isInside(point)) return false;
  }  
  return true;
}

void Intersection::getLightingProperties(double point[3],LightingProperties *props,double normal[3]) {
  /* Simplifying assumption, this function is only called after a
  successfull lineintersection test. Otherwise we would have to
  compute which object is actually the limiting object at this point.
  This is a lot faster than the alternative to compute the correct
  intersecting point, but it is not correct when running with multiple
  threads and not even with a single thread and some special scene
  graphs. */
  lastObject[omp_get_thread_num()]->getLightingProperties(point,props,normal);
}

Inverse::Inverse(Object *o) { object=o; o->reference(); }
Inverse::~Inverse() { object->dereference(); }
double Inverse::lineTest(double origin[3],double direction[3],double maxDistance) {
  return object->lineTest(origin,direction,maxDistance);
}
void Inverse::getNormal(double point[3],double normal[3]) {
  int i;
  object->getNormal(point,normal);
  for(i=0;i<3;i++) normal[i] = -normal[i];  
}
bool Inverse::isInside(double point[3]) {
  return !object->isInside(point);
}
void Inverse::getLightingProperties(double point[3],LightingProperties *props,double normal[3]) {
  int i;

  for(i=0;i<3;i++) normal[i] = -normal[i];  
  object->getLightingProperties(point,props,normal);
  for(i=0;i<3;i++) normal[i] = -normal[i];  
}
