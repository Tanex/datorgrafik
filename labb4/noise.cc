/** \file noise.cc
    \brief Implements functions for computing semi-random noise in 2-5
    dimensions. 
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

#include "noise.h"
#include "stdlib.h"

int randData[5][1024];

void initNoise() {
  int i,j;
  
  for(i=0;i<5;i++) for(j=0;j<1024;j++)
	  randData[i][j] = (rand()<<1) & 0xffff;
}
inline double semiRand(int x) {
  return randData[0][x&0x3ff]/32768.0 - 1.0;
}
inline double semiRand(int x,int y) {
  return randData[1][(randData[0][x&0x3ff]+y)&0x3ff]/32768.0 - 1.0;
}
inline double semiRand(int x,int y,int z) {
  return randData[2][(randData[1][(randData[0][x&0x3ff]+y)&0x3ff]+z)&0x3ff]/32768.0 - 1.0;
}
inline double semiRand(int x,int y,int z,int w) {
  return randData[3][(randData[2][(randData[1][(randData[0][x&0x3ff]+y)&0x3ff]+z)&0x3ff]+w)&0x3ff]/32768.0 - 1.0;
}
inline double semiRand(int x,int y,int z,int w,int h) {
  return randData[4][(randData[3][(randData[2][(randData[1][(randData[0][x&0x3ff]+y)&0x3ff]+z)&0x3ff]+w)&0x3ff]+h)&0x3ff]/32768.0 - 1.0;
}

double noise(double a) {
  a += 2e5;
  int ia=(int) a;
  double frac=a - ia;
  frac=3*frac*frac-2*frac*frac*frac;
  return semiRand(ia)*(1-frac)+semiRand(ia+1)*frac;
}
double noise(double a,double b) {
  a += 3e5; b += 7e5;
  int ia=(int) a, ib=(int) b;
  double fa=a - ia, fb=b - ib;
  fa=3*fa*fa-2*fa*fa*fa;
  fb=3*fb*fb-2*fb*fb*fb;
  double va1=semiRand(ia,ib)*(1-fa)+semiRand(ia+1,ib)*fa;
  double va2=semiRand(ia,ib+1)*(1-fa)+semiRand(ia+1,ib+1)*fa;
  return va1*(1-fb)+va2*fb;
}
double noise(double a,double b,double c) {
  a += 1e5; b += 2e5; c += 3e5;
  int ia=(int) a, ib=(int) b, ic=(int) c;
  double fa=a - ia, fb=b - ib, fc=c - ic;
  fa=3*fa*fa-2*fa*fa*fa;
  fb=3*fb*fb-2*fb*fb*fb;
  fc=3*fc*fc-2*fc*fc*fc;
  double va1=semiRand(ia,ib,ic)*(1-fa)+semiRand(ia+1,ib,ic)*fa;
  double va2=semiRand(ia,ib+1,ic)*(1-fa)+semiRand(ia+1,ib+1,ic)*fa;
  double vb1=va1*(1-fb)+va2*fb;
  double va3=semiRand(ia,ib,ic+1)*(1-fa)+semiRand(ia+1,ib,ic+1)*fa;
  double va4=semiRand(ia,ib+1,ic+1)*(1-fa)+semiRand(ia+1,ib+1,ic+1)*fa;
  double vb2=va3*(1-fb)+va4*fb;
  return vb1*(1-fc)+vb2*fc;
}
double noise(double a,double b,double c,double d) {
  a += 1e5; b += 2e5; c += 3e5; d += 8e4;
  int ia=(int) a, ib=(int) b, ic=(int) c, id=(int) d;
  double fa=a - ia, fb=b - ib, fc=c - ic, fd=d - id;
  fa=3*fa*fa-2*fa*fa*fa;
  fb=3*fb*fb-2*fb*fb*fb;
  fc=3*fc*fc-2*fc*fc*fc;
  fd=3*fd*fd-2*fd*fd*fd;
  double va1=semiRand(ia,ib,ic,id)*(1-fa)+semiRand(ia+1,ib,ic,id)*fa;
  double va2=semiRand(ia,ib+1,ic,id)*(1-fa)+semiRand(ia+1,ib+1,ic,id)*fa;
  double vb1=va1*(1-fb)+va2*fb;
  double va3=semiRand(ia,ib,ic+1,id)*(1-fa)+semiRand(ia+1,ib,ic+1,id)*fa;
  double va4=semiRand(ia,ib+1,ic+1,id)*(1-fa)+semiRand(ia+1,ib+1,ic+1,id)*fa;
  double vb2=va3*(1-fb)+va4*fb;
  double vc1=vb1*(1-fc)+vb2*fc;

  double va5=semiRand(ia,ib,ic,id+1)*(1-fa)+semiRand(ia+1,ib,ic,id+1)*fa;
  double va6=semiRand(ia,ib+1,ic,id+1)*(1-fa)+semiRand(ia+1,ib+1,ic,id+1)*fa;
  double vb3=va5*(1-fb)+va6*fb;
  double va7=semiRand(ia,ib,ic+1,id+1)*(1-fa)+semiRand(ia+1,ib,ic+1,id+1)*fa;
  double va8=semiRand(ia,ib+1,ic+1,id+1)*(1-fa)+semiRand(ia+1,ib+1,ic+1,id+1)*fa;
  double vb4=va7*(1-fb)+va8*fb;
  double vc2=vb3*(1-fc)+vb4*fc;
  
  return vc1*(1-fd)+vc2*fd;
}
