/** \file vector.h
    \brief Declares some miscellaneous. math vector/matrix operations.
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



/* Basic info about the datastructures

   3D and 4D vectors (3D + a homogeneous coordinate) are represented 
   as C arrays of length 3 or 4 and can be passed as parameters efficiently
   since arrays are passed as a pointer, we prefer to use arrays of double floating point
   numbers rather than single floaing point numbers to avoid efficiency issues with
   converting the numbers.

   We have two different forms of matrixes, Matrix3d and Matrix4d which operate on 3D/4D vectors
   respectively.
*/
  
typedef double Matrix4d[4][4];
typedef double Matrix3d[3][3];

/*                         */
/* Basic vector operations */
/*                         */

void assign(const float A[3],float C[3]);                           /* Copy content of vector A to vector C */
void assign(const double A[3],double C[3]);                         /* Copy content of vector A to vector C */
void crossProduct(const double A[3],const double B[3],double C[3]); /* Assign crossproduct of A and B to vector C */   
double dotProduct(const double A[3],const double B[3]);           /* Return the dotproduct of two vectors */
void add(const double A[3],const double B[3],double C[3]);          /* Assign C the value of A + B */
void sub(const double A[3],const double B[3],double C[3]);                /* Assign C the value of A - B */
void normalize(double[3]);                                          /* Destructivly normalize vector, ie. force to unit length */
double length(double[3]);                                           /* Give length of vector */
void zero(double[3]);                                               /* Assign vector the value zero */

/*                         */
/* Basic matrix operations */
/*                         */

/** Print out a matrix in a human readable form */
void debugMatrix(Matrix4d);                                         
/** Homogeneis A and assign to C */
void homogenise(double A[4],double C[3]);                           
/** Assign to C the value M * A */
void useMatrix(Matrix4d M,const double A[4],double C[4]);           
/** Assign to C the value M * A */
void useMatrix(Matrix3d,const double A[3],double C[3]);             
/** Assign to C the value M * <x,y,z> */
void useMatrix(Matrix4d A,const double x,const double y,const double z,double C[3]);    
/** Assign to C the value M * <x,y,z> */
void useMatrix(Matrix4d A,const double x,const double y,const double z,float C[3]);   
void identityMatrix(Matrix4d);                                      /* Reset a matrix to the identity matrix */
void assign(const Matrix4d A,Matrix4d C);                           /* Copy matrix A to matrix C */
void matrixMult(Matrix4d A,Matrix4d B,Matrix4d C);                  /* Assign C the value A * B */

void rotateMatrixX(double,Matrix4d);                                      /* Rotate matrix around the X-axis AFTER the original transformation */
void rotateMatrixY(double,Matrix4d);                                      /* Rotate matrix around the Y-axis AFTER the original transformation */
void rotateMatrixZ(double,Matrix4d);                                      /* Rotate matrix around the Z-axis AFTER the original transformation */
void translateXYZ(double x,double y,double z,Matrix4d);             /* Add a translation to the given matrix. AFTER the original transformation */

