/** \file main.cc
    \brief The main entry point and code skeleton for a raytracer. 
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

/** \mainpage
    This lab skeleton consisitutes of a a simple raytracer with a few
    primitive classes for different objects and materials. Although,
    or perhaps because of, it is a rather simplistic raytracer, modern
    computers are fast enough to render the simple example scenes used
    here more or less in realtime.

    In order to create and render a scene a scenegraph is created by
    adding objects (raytracer::addObject) to the raytracer. Since many
    objects can contain other objects (eg. Transform objects or
    Intersection objects) this creates a hierarchical graph of
    objects. To have animated and interactive scenes, the main program
    must create the scenes and later change properties of them between
    different frames to create the illustion of animation. One example
    is by eg. modifying the translation values of a translation object
    contanining other objects. 

    For documentation of the different available primitive objects see
    the class hierarchy. For examples on how to use the raytracer see
    the existing skeleton code in main.cc

    The
    raytracer uses multiple threads to render a scene with as many
    processors as are available on the local machine as possible. See
    the variable nThreads to change this manually. 

    To start, see the <a href="annotated.html"> class list </a> or <a
    href="hierarchy.html"> class hierarchy </a>. 
 */

#include "general.h"
#include "camera.h"
#include "raytracer.h"
#include "sphere.h"
#include "plane.h"
#include "light.h"
#include "transform.h"
#include "noise.h"
#include "csg.h"

#include "WoodMaterial.h"

/* Prototype declarations */
void doRedraw();
void doKeyboard(int,int,int);
void tick(double);
void *renderingThread(void *arg);
void renderPixels(int offset,int skip);

/* Important global variables */
int screenWidth, screenHeight, isRunning;
double gTime, simulationSpeed=5.0;
SDL_Surface *screen;

double cameraOrbit[2]={0.0,0.0};

Raytracer *raytracer;
Transform *object1, *object2;

/** Used for debugging, note that we drop down to singel threaded mode
    when we are using this per-pixel debugging. Thus any threading
    related bugs will _NOT_ be visible using this method.
*/
int debugPixelX=-1, debugPixelY, debugThisPixel=0;
int debugIndentation=0;

int main(int argc,char **args) {
  SDL_Event event;
  int i;

  /* Initialize SDL and create window */
  if(SDL_Init(SDL_INIT_VIDEO) == -1) {
    printf("Failed to initialize SDL. Error '%s'\n",SDL_GetError());
    exit(-1);
  }
  atexit(SDL_Quit);
  SDL_WM_SetCaption("Datorgrafik","Datorgrafik");

  screenWidth=320; screenHeight=240;
  screen = SDL_SetVideoMode(screenWidth, screenHeight, 32, SDL_SWSURFACE); /* Add flag SDL_FULLSCREEN if you like, but be carefull! */
  if(!screen) {
    printf("Failed to open screen in %dx%dx%dbpp mode\n",screenWidth,screenHeight,32);
    exit(0);
  }

  /* Construct the world */
  initNoise();
  raytracer=new Raytracer();
  raytracer->setCamera(new Camera());  
  
  /* Add a positional light source */
  double lightCol[3] = { 1.0, 1.0, 1.0 };
  double lightPos[3] = { 1.0, 3.0, 1.0 };
  Light *light = new Light(lightPos,lightCol);
  raytracer->addLight(light);

  /* Setup ambient lighting in the scene */
  double ambientLight[3] = {0.2,0.2,0.2};
  raytracer->setAmbientLight(ambientLight);

  /* A ground plane */
  double planeNormal[3] = { 0.0, 1.0, 0.0 };
  Plane *floor = new Plane(planeNormal,-0.5);
  raytracer->addObject(floor);

  /* This sets the ground plane to use a checkerboard material */
  LightingProperties floorA = {{0.8,0.8,0.8},{0.8,0.8,0.8},{1.0,1.0,1.0}, 10, {0.5,0.5,0.5}};
  LightingProperties floorB = {{0.8,0.4,0.4},{0.8,0.4,0.4},{1.0,0.5,0.5}, 10, {0.5,0.25,0.25}};
  floor->setMaterial(new CheckerboardMaterial(1.0,&floorA,&floorB));

  /* The first sphere */
  Sphere *sphere1=new Sphere(0.5);
  LightingProperties ballProps = {{0.8,0.8,0.4},{0.8,0.8,0.4},{2.0,2.0,1.0}, 10, {0.5, 0.5, 0.0}};
  LightingProperties ballProps2 = {{0.8,0.8,0.8},{0.8,0.8,0.8},{2.0,2.0,2.0}, 10, {0.5, 0.5, 0.5}};
  //LightingProperties ballProps2 = {{0.8,0.8,0.8},{0.8,0.8,0.8},{2.0,2.0,2.0}, 10, {0.0, 0.0, 0.0}};
  //sphere1->setMaterial(new SimpleMaterial(&ballProps));
  sphere1->setMaterial(new WoodMaterial());
  object1 = new Transform(sphere1);
  raytracer->addObject(object1);

  /* A second sphere */
  Sphere *sphere2 = new Sphere(0.5);
  sphere2->setMaterial(new SimpleMaterial(&ballProps2));  
  object2 = new Transform(sphere2);

  /* Enable this to use an intersection object instead as the second
     sphere */
  double plane2Normal[3] = { 1.0, 0.0, 0.0 };
  Plane *plane2 = new Plane(plane2Normal,0.0);
  plane2->setMaterial(new CheckerboardMaterial(1.0,&floorA,&floorB));
  Intersection *intersection = new Intersection();
  intersection->addObject(sphere2);
  intersection->addObject(plane2);
  object2 = new Transform(intersection);
  raytracer->addObject(object2);

  /* This sets the ground plane to use a simple marble material */
  LightingProperties marble0 = {{0.6,0.8,0.6},{0.6,0.8,0.6},{1.0,1.0,1.0}, 10, {0.5,0.5,0.5}};
  LightingProperties marble1 = {{0.2,0.4,0.2},{0.2,0.4,0.2},{1.0,1.0,1.0}, 10, {0.5,0.5,0.5}};
  MaterialMap *map=new MaterialMap(MaterialMap::Noise);
  map->add(-0.1,&marble0);
  map->add(0.0,&marble1);
  map->add(0.1,&marble0);
  floor->setMaterial(map);
  

  /* Main event loop */
  isRunning=1;
  double oldTime, newTime, timeDelta;
  oldTime=SDL_GetTicks()/1000.0;
  while(isRunning) {
    /* Yield processor time to other processes, otherwise windows
       can hang */
    SDL_Delay(1);

    /* Compute how much time has elapsed since last frame */
    newTime=SDL_GetTicks()/1000.0;
    timeDelta = newTime - oldTime;
    oldTime=newTime;

    /* Update world */
    tick(timeDelta);

    /* Draw world */
    doRedraw();
    SDL_UpdateRect(screen,0,0,screenWidth,screenHeight);

    /* Process any events that have occured */
    while(SDL_PollEvent(&event)) 
      switch(event.type) {
      case SDL_QUIT: isRunning=0; break;
      case SDL_MOUSEBUTTONDOWN:
	/* do something here. e->button is mouse button, e->x and e->y is coordinate of mouse */
	SDL_GetMouseState(&debugPixelX,&debugPixelY);
	break;
      case SDL_KEYDOWN:
	if(event.key.keysym.sym == SDLK_ESCAPE) {
	  isRunning=0; break;
	} else {
	  int mouseX, mouseY;
	  Uint8 mouseButtonState=SDL_GetMouseState(&mouseX,&mouseY);
	  doKeyboard(event.key.keysym.sym,mouseX,mouseY);
	}
      }
  }

  /* Free raytracer, this also removes all objects referenced by it */
  delete raytracer;

  /* Exit */
  exit(0);
}

/* Handle keyboard. */
void doKeyboard(int key,int mouseX,int mouseY) {
  if(key == 27) exit(0);
}

void putPixel(int x,int y,int r,int g,int b) {
  Uint32 colour;
  colour = SDL_MapRGB(screen->format,(Uint8) r,(Uint8) g,(Uint8) b);
  *((Uint32*)screen->pixels+y*screen->pitch/4+x) = colour;
}

/* Called when the screen needs to be redrawn */
void doRedraw() {
  int i;
  double vec[3];
  Camera *camera = raytracer->getCamera();

  /* Set camera up vector */
  vec[0]=0.0; vec[1]=1.0; vec[2]=0.0; 
  camera->setUp(vec);
  /* Place camera 4 units away in orbit around origo, let mouse
     movements rotate camera. */
  vec[0]=4.0*sin(cameraOrbit[0])*cos(cameraOrbit[1]); vec[1]=4.0*sin(cameraOrbit[1]); vec[2]=4.0*cos(cameraOrbit[0])*cos(cameraOrbit[1]);
  camera->setOrigin(vec);
  /* Set focus of camera to be in origo */
  zero(vec);
  camera->setFocus(vec);

  /* Set the position/scale of the first sphere so it appears to be
     bouncing  (infinitly long). */
  object1->identity();
  double ypos = fabs(cos(gTime*M_PI/5.0))-0.2;
  if(ypos < 0.0)
    object1->scale(1.0,1.0-(-ypos),1.0);
  object1->rotateX(gTime);
  object1->translate(-0.6,ypos,0.0);

  /* Set the position/scale of the second sphere so it appears to be
     bouncing  (infinitly long). */
  object2->identity();
  double ypos2 = 0.8*fabs(cos(gTime*M_PI/4.0))-0.2;
  if(ypos2 < 0.0)
    object2->scale(1.0,1.0-(-ypos2),1.0);
  object2->translate(+0.6,ypos2,0.0);

  if(debugPixelX != -1) {
    /* User has clicked on the screen, run all the rendring in
       non-threaded mode and set the debugThisPixel variable to true
       for one of the pixels. */
    printf("Debugging frame for X=%d, Y=%d\n",debugPixelX,debugPixelY);
    double rgb[3];
    debugThisPixel=1;
#pragma omp parallel default(shared) private(i)
#pragma omp for schedule(guided) 
    for(i=0;i<1;i++) // Make a dummy for loop to make sure that OpenMP is used always within the raytracing parts
      raytracer->raytrace(debugPixelX,debugPixelY,rgb);
    printf("SCREEN <- %.3f %.3f %.3f\n",rgb[0],rgb[1],rgb[2]);
    debugThisPixel=0;
    debugPixelX=-1;
  }

  int pixels=screenWidth*screenHeight;
#pragma omp parallel default(shared) private(i)
#pragma omp for schedule(guided) 
  for(i=0;i<pixels;i++) {
    int x=i%screenWidth;
    int y=i/screenWidth;
    double rgb[3];
    raytracer->raytrace(x,y,rgb);

    for(int j=0;j<3;j++) if(rgb[j] > 1.0) rgb[j]=1.0; else if(rgb[j] < 0.0) rgb[j]=0.0;
    /* Note that we do not need to protect the putPixel call with a
       semaphore since we are guaranteed to write to different memory
       addresses every time. This would not hold if we where not
       running in 32bpp mode. */
    putPixel(x,y,(int)(rgb[0]*255.0),(int)(rgb[1]*255.0),(int)(rgb[2]*255.0));
  }
}

void printDebugIndentation() { int i; for(i=0;i<debugIndentation;i++) printf(" "); }
void tick(double dt) { 
  /* This measures the elapsed time in seconds since the start of
     the program. 
  */
  gTime+=dt;
  
  static double fps=1.0;
  static int cnt=0;
  fps = 0.95*fps + 0.05*1.0/dt;
  if((++cnt) % 10 == 0) 
    printf("Average framerate: %3.1ffps\n",fps);

  /* Some usefull functions you might want to use...
     
     Uint8 *keystate = SDL_GetKeyState(NULL);
     index this array by id. Like: if(keystate['a'] || keystate[SDL_LEFT] ...)
     
     if(SDL_GetModState() & KMOD_LSHIFT) { true if left shift is pressed }

     Uint8 mouseButtonState=SDL_GetMouseState(&mouseX,&mouseY);
     Gets the position and state (mouse buttons) of the mouse
  */

  /* Let player orbit the camera around the plane using the mouse
     buttons and dragging */
  int mouseX,mouseY;
  static int oldMouseX=0, oldMouseY=0;
  Uint8 mouseButtonState=SDL_GetMouseState(&mouseX,&mouseY);
  if(mouseButtonState) {
    cameraOrbit[0] += (mouseX - oldMouseX) / 100.0;
    cameraOrbit[1] += (mouseY - oldMouseY) / 100.0;
  }
  oldMouseX = mouseX; oldMouseY = mouseY;
}
