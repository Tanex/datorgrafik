/* lab-1.1
   Copyright (C) 2011 Mathias Broxvall

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
#include "glUtil.h"

/** Container for all OpenGL related resources such as textures and shaders that are used in the labs */
class Resources {
public:
  Resources();
  ~Resources(); 

  /** Function to (re)load the content of all resources such as textures and shaders */
  void reload();

  /* note: in a "real" program, you should protect all the resources in
     private variables with 'inlined' accessors (but not modifier)
     functions... but this is not a course about proper C++
     programming so we're gonna be lazy for now */
  GLuint fragmentShader, vertexShader, shaderProgram;

  /** Mark if the loaded shader code is "ok" or not - do not attempt to use any shaders or 
      draw anything if false. */
  int isOk;
};

/** Container for the world state representation. Eg., camera position, players/enemies/simulation data */
class World {
public:
  World();
  ~World();

  /** Draws all graphics, using the given OpenGL resources */
  void doRedraw(Resources*);
  /** Updates the world */
  void tick(double deltaTime);
  /** Handles keyboard input */
  void doKeyboard(int,int,int);
private:
};

/* Important global variables */
int screenWidth, screenHeight, isRunning;
Resources *resources;
World *world;


Resources::Resources() {
  /* Create shader and program objects, attach shaders to program */
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  reload();
}
Resources::~Resources() {
  /* Free shader and program objects */
  glDeleteProgram(shaderProgram);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void Resources::reload() {
  isOk=0;

  /* Recompile shader objects, mark resources as not-ok if any shader fails */
  if(compileShader(fragmentShader, "fragmentShader.fs", "fragmentShader.log")) return;
  if(compileShader(vertexShader, "vertexShader.vs", "vertexShader.log")) return;
  /* Link all shader programs */
  if(linkProgram(shaderProgram,"program.log")) return;
  isOk=1;
}

World::World() {
}

void World::doRedraw(Resources *r) {
  GLuint error;

  /* If the resources (shaders etc.) are not ok, then don't draw anything */
  if(!r->isOk) return;

  /* Setup the viewport to use all of the SDL window */
  glViewport(0,0,(GLsizei) screenWidth,(GLsizei) screenHeight); 

  glClearColor(0.0,0.0,0.0,0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  /* Specify four vertices, each consisting of four XYZW points. */
  float sw=screenWidth, sh=screenHeight;   
  GLfloat vertices[4][4] = {{-1.0, -1.0, 0.0, 1.0},
			    {1.0 , -1.0, 0.0, 1.0}, 
			    {1.0 , +1.0 , 0.0, 1.0},
			    {-1.0, +1.0 , 0.0, 1.0}};
  /* Give colours for each such vertice */
  GLfloat colours[4][3] = {{0.5, 0.5, 0.0}, 
			   {0.5, 0.5, 0.0},
			   {0.5, 0.5, 0.0},
			   {0.5, 0.5, 0.0}};
  /* This is a list of graphic primitives (as triangles), that reference the above vertices */
  GLuint indices[2][3] = {{0,1,2},{2,3,0}};

  glUseProgram(r->shaderProgram);

  /* Attrib, N-Elements, Type, Normalize, Stride, Pointer */
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*) &vertices);  
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*) &colours);
  glEnableVertexAttribArray(1);

  /* (a) This draws the two triangles given by vertex 0,1,2 and 2,3,0 */
  glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, &indices[0][0]);
  /* (b) This alternative draws the four vertices directly as a rectangle on the screen     
     glDrawArrays(GL_QUADS, 0, 4);
  */


  if((error=glGetError()) != GL_NO_ERROR) {
    fprintf(stderr,"GL Error: %s\n",gluErrorString(error));
  }
}

void World::doKeyboard(int key,int mouseX,int mouseY) {
  /** TODO - implement a keyhandler that does something interesting when given inputs */
  switch(key) {
  }
}

void World::tick(double deltaTime) { }


/** Main function, initializing the program and performing the draw/event/tick loop */
int main(int argc,char **args) {
  SDL_Event event;
  SDL_MouseButtonEvent *e=(SDL_MouseButtonEvent*)&event;
  
  /* Initialize SDL and create window */
  if(SDL_Init(SDL_INIT_VIDEO) == -1) {
    printf("Failed to initialize SDL. Error '%s'\n",SDL_GetError());
    exit(-1);
  }
  atexit(SDL_Quit);
  SDL_WM_SetCaption("Datorgrafik","Datorgrafik");
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
  screenWidth=800; screenHeight=600;

  /* Add flag SDL_FULLSCREEN if you like, but be carefull since this
     may crash the computer if you have any bugs in your program! */
  SDL_SetVideoMode(screenWidth, screenHeight, 32, SDL_SWSURFACE | SDL_OPENGL);

  if(glewInit() != GLEW_OK) {
    fprintf(stderr,"WARNING - failed to initialize GLEW. This will most likely fail your program\n");
  }

  resources = new Resources();
  world = new World();

  isRunning=1;
  double oldTime, newTime, timeDelta;
  oldTime=SDL_GetTicks()/1000.0;
  while(isRunning) {
    /* Yield some processor time to other processes, otherwise windows might hang */
    SDL_Delay(1);

    /* Compute how much time has elapsed since last frame */
    newTime=SDL_GetTicks()/1000.0;
    timeDelta = newTime - oldTime;
    oldTime=newTime;

    /* Update world */
    world->tick(timeDelta);

    /* Draw world */   
    world->doRedraw(resources);
    glFlush();
    SDL_GL_SwapBuffers();

    /* Process any events that have occured */
    while(SDL_PollEvent(&event)) 
      switch(event.type) {
      case SDL_QUIT: isRunning=0; break;
      case SDL_MOUSEBUTTONDOWN:
	/* do something here. e->button is mouse button, e->x and e->y is coordinate of mouse */
	break;
      case SDL_KEYDOWN:
	if(event.key.keysym.sym == SDLK_ESCAPE) {
	  isRunning=0; break;
	} else {
	  int mouseX, mouseY;
	  Uint8 mouseButtonState=SDL_GetMouseState(&mouseX,&mouseY);
	  switch(event.key.keysym.sym) {
	  case 27:
	    exit(0);
	    break;
	  case 'l':
	    resources->reload();
	    break;
	  default:
	    world->doKeyboard(event.key.keysym.sym,mouseX,mouseY);
	  } 
	}
      }
  }
  SDL_Quit();
  return -1;
}

#ifdef WINDOWS
#include "stdafx.h"
int _tmain(int argc, _TCHAR* argv[]) {
  main(0,NULL);
  return 0;
}
#endif
