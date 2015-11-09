/* general.h
   Contains common type definitions, includes, etc. for all labs
*/

/* First, standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef LINUX
#ifndef DARWIN
#include <windows.h>
#endif
#endif

/* OpenGL includes */
#include "GL/glew.h"

/* SDL related includes */
#include "SDL.h"
#include "SDL_mouse.h"
#include "SDL_keyboard.h"
#include "SDL_keysym.h"
#include "SDL_image.h"

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

#ifndef M_PI
#define M_PI 3.14159265
#endif
