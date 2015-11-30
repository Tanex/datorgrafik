/** \file vector.cc
    \brief Implements some miscellaneous. math vector/matrix operations.
*/
/* Made by Mathias Broxvall 

   Copyright (C) 2000-2004  Mathias Broxvall

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

#include <stdio.h>
#include <math.h>
#include "general.h"
#include "vector.h"

/********************/
/* Vector operations */
/********************/

/* C <- A + B */
void add(const double A[3],const double B[3],double C[3]) {
  for(int i=0;i<3;i++) C[i] = A[i] + B[i];
}

/* C <- A - B */
void sub(const double A[3],const double B[3],double C[3]) {
  for(int i=0;i<3;i++) C[i] = A[i] - B[i];
}
/* C <- C * 1 / |C| */
void normalize(double C[3]) {
  double l = sqrt(C[0]*C[0]+C[1]*C[1]+C[2]*C[2]);
  C[0] /= l; C[1] /= l; C[2] /= l;
}
/* |A| */
double length(double A[3]) {
  return sqrt(A[0]*A[0]+A[1]*A[1]+A[2]*A[2]);
}
/* C <- A x B */
void crossProduct(const double A[3],const double B[3],double C[3]) {
  C[0] = A[1] * B[2] - A[2] * B[1];
  C[1] = A[2] * B[0] - A[0] * B[2];
  C[2] = A[0] * B[1] - A[1] * B[0];
}

/* <- A . B */
double dotProduct(const double A[3],const double B[3]) {
  return A[0]*B[0] + A[1]*B[1] + A[2]*B[2];
}

/*********************/
/* Matrix operations */
/*********************/

void debugMatrix(Matrix4d m) {
  printf("%f \t%f \t%f \t%f\n",m[0][0],m[0][1],m[0][2],m[0][3]);
  printf("%f \t%f \t%f \t%f\n",m[1][0],m[1][1],m[1][2],m[1][3]);
  printf("%f \t%f \t%f \t%f\n",m[2][0],m[2][1],m[2][2],m[2][3]);
  printf("%f \t%f \t%f \t%f\n",m[3][0],m[3][1],m[3][2],m[3][3]);
}

/* C <- A * B */
void matrixMult(Matrix4d A, Matrix4d B, Matrix4d C) {
  int i,j,k;
  for(i=0;i<4;i++)
    for(j=0;j<4;j++) {
      C[i][j] = 0.0;
      for(k=0;k<4;k++)
	C[i][j] += A[i][k] * B[k][j];
    }
}

void homogenise(double A[4],double C[3]) {
  C[0] = A[0] / A[3];
  C[1] = A[1] / A[3];
  C[2] = A[2] / A[3];
}

/* C <- A(B) */
void useMatrix(Matrix4d A,const double B[4],double C[4]) {
  int i,k;
  for(i=0;i<4;i++) {
    C[i] = 0.0;
    for(k=0;k<4;k++)
      C[i] += A[i][k] * B[k];
  }
  //  for(k=0;k<4;k++) C[k] /= C[3];
}

/* C <- A(B) */
void useMatrix(Matrix3d A,const double B[3],double C[3]) {
  int i,k;
  for(i=0;i<3;i++) {
    C[i] = A[i][3];
    for(k=0;k<3;k++)
      C[i] += A[i][k] * B[k];
  }
}

/* C <- m(<x,y,z>) */
/* This version assumes that we have H=1 and performs complete
   transform. */
void useMatrix(Matrix4d m,const double x,const double y,const double z,double C[3]) {
  int i;
  for(i=0;i<3;i++) {
    C[i] = m[i][3];
    C[i] += m[i][0] * x;
    C[i] += m[i][1] * y;
    C[i] += m[i][2] * z;
  }
}
/* C <- m(<x,y,z>) */
/* This version assumes that we have H=1 and performs complete
   transform. */
void useMatrix(Matrix4d m,const double x,const double y,const double z,float C[3]) {
  int i;
  for(i=0;i<3;i++) {
    C[i] = m[i][3];
	C[i] += m[i][0] * x;
	C[i] += m[i][1] * y;
	C[i] += m[i][2] * z;
  }
}


/* C <- A */
void assign(const Matrix4d A,Matrix4d C) {
  int i,j;
  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      C[i][j] = A[i][j];
}

/* C <- A */
void assign(const double A[3],double C[3]) { C[0] = A[0]; C[1] = A[1]; C[2] = A[2]; }

/* C <- A */
void assign(const float A[3],float C[3]) { C[0] = A[0]; C[1] = A[1]; C[2] = A[2]; }

void identityMatrix(Matrix4d m) {
  int i,j;
  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      m[i][j] = i==j?1.0:0.0;
}

void rotateMatrixX(double v,Matrix4d m) {
  Matrix4d mr = {
    {1.0,    0.0,    0.0,      0.0},
    {0.0,    cos(v), -sin(v),   0.0},
    {0.0,   +sin(v), cos(v),   0.0},
    {0.0,    0.0,    0.0,      1.0}};
  Matrix4d morig;
  assign(m,morig);
  matrixMult(mr,morig,m);
}
void rotateMatrixY(double v,Matrix4d m) {
  Matrix4d mr = {
    {cos(v), 0.0,    +sin(v),   0.0},
    {0.0,    1.0,    0.0,      0.0},
    {-sin(v), 0.0,   cos(v),   0.0},
    {0.0,    0.0,    0.0,      1.0}};
  Matrix4d morig;
  assign(m,morig);
  matrixMult(mr,morig,m);
}
void rotateMatrixZ(double v,Matrix4d m) {
  Matrix4d mr = {
    {cos(v),  -sin(v),  0.0,   0.0},
    {+sin(v), cos(v),  0.0,   0.0},
    {0.0,     0.0,     1.0,   0.0},
    {0.0,     0.0,     0.0,   1.0}};
  Matrix4d morig;
  assign(m,morig);
  matrixMult(mr,morig,m);
}
void translateXYZ(double x,double y,double z,Matrix4d m) {
  Matrix4d mt = {
	{1.0,     0.0,     0.0,     x},
	{0.0,     1.0,     0.0,     y},
	{0.0,     0.0,     1.0,     z},
	{0.0,     0.0,     0.0,   1.0}};
  Matrix4d morig;
  assign(m,morig);
  matrixMult(mt,morig,m);
}

void zero(double v[3]) { v[0]=v[1]=v[2]=0.0; }
