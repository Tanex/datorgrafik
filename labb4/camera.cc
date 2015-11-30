/** \file camera.cc
    \brief Implements all methods for the Camera class. 
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
#include "camera.h"

Camera::Camera() {
  zero(origin); zero(up); zero(right); zero(forward);
  tanFovX=1.0;
  tanFovY=240./320.*tanFovX;
}
void Camera::setOrigin(double v[3]) { assign(v,origin); }
void Camera::setUp(double v[3]) { assign(v,up); normalize(up); }
void Camera::setRight(double v[3]) { assign(v,right); normalize(right); }
void Camera::setForward(double v[3]) { assign(v,forward); normalize(forward); }

void Camera::setFocus(double focus[3]) {
  int i;
  double dot;
  sub(focus,origin,forward);
  normalize(forward);
  dot=dotProduct(forward,up);
  if(dot == 1.0) { 
    printf("Error - invalid use of setFocus. Focal point coincides with up vector.\n");
    exit(0);
  }
  /*printf("dot = %3.1f\n",dot);*/
  for(i=0;i<3;i++) up[i] -= dot*forward[i];
  normalize(up);
  crossProduct(forward,up,right);
  /*printf("forward: %3.1f %3.1f %3.1f\n",forward[0],forward[1],forward[2]);
  printf("up: %3.1f %3.1f %3.1f\n",up[0],up[1],up[2]);
  printf("right: %3.1f %3.1f %3.1f\n",right[0],right[1],right[2]);*/
}

void Camera::getPixelRay(double x,double y,double origin[3],double direction[3]) {
  int i;
  assign(this->origin,origin);
  for(i=0;i<3;i++) direction[i]=forward[i]+up[i]*(0.5-y)*tanFovY+right[i]*(x-0.5)*tanFovX;
  normalize(direction);
}
