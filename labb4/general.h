/** \file general.h
   \brief Contains common type definitions, includes, etc. for all
   labs
*/
/*
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

/* First, standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/*#include <unistd.h>*/
#include <set>

#ifndef LINUX
#ifndef DARWIN
#include <windows.h>
#endif
#endif

/* SDL related includes */
#include "SDL.h"
#include "SDL_mouse.h"
#include "SDL_keyboard.h"
#include "SDL_keysym.h"
#include "SDL_image.h"

/* Vector operations */
#include "vector.h"

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

#ifndef M_PI
#define M_PI 3.14159265
#endif

extern int debugThisPixel, debugIndentation;
extern void printDebugIndentation();

#define MAX_OMP_THREADS 32
