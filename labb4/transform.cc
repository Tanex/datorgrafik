/** \file transform.cc
    \brief Implements all methods for the Transform class.
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
#include "transform.h"

Transform::Transform(Object *child) :Object() { 
  child->reference();
  identityMatrix(forward); 
  identityMatrix(inverse);  
  this->child=child;
}
Transform::~Transform() {
  child->dereference();
}
double Transform::lineTest(double origin[3],double direction[3],double maxDistance) {
  double newOrigin[4], newDirection[4];
  /* Use inverse transformation matrix on origin with H=1 */
  /* Note, we are ignoring generated homogeneous coordinate after
     transformation so the multiplication is not done to save speed. */
  newOrigin[0]=inverse[0][0]*origin[0]+inverse[0][1]*origin[1]+inverse[0][2]*origin[2]+inverse[0][3];
  newOrigin[1]=inverse[1][0]*origin[0]+inverse[1][1]*origin[1]+inverse[1][2]*origin[2]+inverse[1][3];
  newOrigin[2]=inverse[2][0]*origin[0]+inverse[2][1]*origin[1]+inverse[2][2]*origin[2]+inverse[2][3];

  /* Use inverse transformation matrix on direction with H=0 */
  newDirection[0]=inverse[0][0]*direction[0]+inverse[0][1]*direction[1]+inverse[0][2]*direction[2];
  newDirection[1]=inverse[1][0]*direction[0]+inverse[1][1]*direction[1]+inverse[1][2]*direction[2];
  newDirection[2]=inverse[2][0]*direction[0]+inverse[2][1]*direction[1]+inverse[2][2]*direction[2];
  
  /* The correct way of doing it: */
  //double distanceScale = length(newDirection);
  //return child->lineTest(newOrigin,newDirection,maxDistance/distanceScale) / distanceScale;

  /* Slightly more efficient way of doing it */
  return child->lineTest(newOrigin,newDirection,maxDistance);  
}
void Transform::getNormal(double point[3],double normal[3]) {
  double newPoint[3], newNormal[3];
  /* Compute target point using inverse matrix and H=1 */
  newPoint[0]=inverse[0][0]*point[0]+inverse[0][1]*point[1]+inverse[0][2]*point[2]+inverse[0][3];
  newPoint[1]=inverse[1][0]*point[0]+inverse[1][1]*point[1]+inverse[1][2]*point[2]+inverse[1][3];
  newPoint[2]=inverse[2][0]*point[0]+inverse[2][1]*point[1]+inverse[2][2]*point[2]+inverse[2][3];
  /* Get child normal */
  child->getNormal(newPoint,normal);
  /* Translate normal back using forward matrix and H=0 */
  newNormal[0]=forward[0][0]*normal[0]+forward[0][1]*normal[1]+forward[0][2]*normal[2];
  newNormal[1]=forward[1][0]*normal[0]+forward[1][1]*normal[1]+forward[1][2]*normal[2];
  newNormal[2]=forward[2][0]*normal[0]+forward[2][1]*normal[1]+forward[2][2]*normal[2];
  assign(newNormal,normal);
}
bool Transform::isInside(double point[3]) {
  double newPoint[3];
  /* Compute target point using inverse matrix and H=1 */
  newPoint[0]=inverse[0][0]*point[0]+inverse[0][1]*point[1]+inverse[0][2]*point[2]+inverse[0][3];
  newPoint[1]=inverse[1][0]*point[0]+inverse[1][1]*point[1]+inverse[1][2]*point[2]+inverse[1][3];
  newPoint[2]=inverse[2][0]*point[0]+inverse[2][1]*point[1]+inverse[2][2]*point[2]+inverse[2][3];
  /* Ask child if inside */
  return child->isInside(newPoint);
}

void Transform::identity() {
  identityMatrix(forward); 
  identityMatrix(inverse);  
}

void Transform::translate(double dx,double dy,double dz) {
  Matrix4d M,M2;
  identityMatrix(M);
  M[0][3]=dx;
  M[1][3]=dy;
  M[2][3]=dz;
  matrixMult(M,forward,M2);
  assign(M2,forward); 
  computeInverseTransform();
}

void Transform::scale(double sx,double sy,double sz) {
  Matrix4d M,M2;
  identityMatrix(M);
  M[0][0]=sx;
  M[1][1]=sy;
  M[2][2]=sz;
  matrixMult(M,forward,M2);
  assign(M2,forward);
  computeInverseTransform();
}
void Transform::rotateX(double rad) {
  rotateMatrixX(rad,forward);
  computeInverseTransform();  
}
void Transform::rotateY(double rad) {
  rotateMatrixY(rad,forward);
  computeInverseTransform();  
}
void Transform::rotateZ(double rad) {
  rotateMatrixZ(rad,forward);
  computeInverseTransform();  
}

//#define DEBUG_INVERSE
void Transform::computeInverseTransform() {
  /* Use gauss-jordan's algorithm to invert the forward matrix to create the inverse matrix */      
  int i,j,k;
  double M[4][8]; /* Will hold the composite [ forward | identity ]
		     and be solved */

  for(i=0;i<4;i++)
    for(j=0;j<4;j++) {
      M[i][j]=forward[i][j];
      M[i][j+4] = (i==j?1.0:0.0);
    }
  /* TODO - add pivot operations to yield better nummerical stability */
  /* Step 1: eliminate M[i][i] from M[i+1][..] and down */
  for(i=0;i<3;i++) {
    for(j=i+1;j<4;j++) {
      for(k=i+1;k<8;k++)
	M[j][k] -= M[j][i]/M[i][i] * M[i][k];      
      M[j][i]=0.0;
    }
  }
  /* Step 2: substitute upwards */
  for(i=3;i>=0;i--) {
    for(k=i+1;k<8;k++) 
      M[i][k] /= M[i][i];
    M[i][i] = 1.0;
    for(j=0;j<i;j++) {
      for(k=i+1;k<8;k++)
	M[j][k] -= M[i][k] * M[j][i];
      M[j][i] = 0.0;
    }
  }
  /* The result is now in M[4..7][0..3] */
  for(i=0;i<4;i++)
    for(j=0;j<4;j++) {
      inverse[i][j] = M[i][j+4];
    }
}

void Transform::getLightingProperties(double point[3],LightingProperties *props,double normal[3]) {
  double newPoint[3];
  /* Use inverse transformation matrix on point with H=1 */
  /* Note, we are ignoring generated homogeneous coordinate after
     transformation so the multiplication is not done to save speed. */
  newPoint[0]=inverse[0][0]*point[0]+inverse[0][1]*point[1]+inverse[0][2]*point[2]+inverse[0][3];
  newPoint[1]=inverse[1][0]*point[0]+inverse[1][1]*point[1]+inverse[1][2]*point[2]+inverse[1][3];
  newPoint[2]=inverse[2][0]*point[0]+inverse[2][1]*point[1]+inverse[2][2]*point[2]+inverse[2][3];

  if(material)
    material->getLightingProperties(newPoint,props,normal);
  else
    child->getLightingProperties(newPoint,props,normal);
}
