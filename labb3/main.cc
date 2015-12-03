/* lab-2.1
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
#include "glm/glm.hpp"
#include "gtc/matrix_transform.hpp"

/** Container for all OpenGL related resources such as textures and shaders that are used in the labs */
class Resources {
//part 2
private:
	void calcArraySizes();
	void traverseAndCount(AC3DObject* node);

	void modelTraversal(AC3DObject* node);
	void modelLeaf(AC3DObject* obj);
	int sizeOfVertexBuffer;
	int sizeOfTriIndiceBuffer;
	int sizeOfQuadIndiceBuffer;

public:
	glm::vec4* vertexBuffer;
	glm::ivec3* triIndiceBuffer;
	glm::ivec4* quadIndiceBuffer;
	void buildBuffers(AC3DObject* node);
	int getVertexBufSize() { return sizeOfVertexBuffer; }
	int getTriIndicesBufSize() { return sizeOfTriIndiceBuffer; }
	int getQuadIndicesBufSize() { return sizeOfQuadIndiceBuffer; }

  Resources();
  ~Resources();


  /** Function to (re)load the content of all resources such as textures and shaders */
  void reload();

  /* note: in a "real" program, you should protect all the resources in
     private variables with 'inlined' accessors (but not modifier)
     functions... but this is not a course about proper C++
     programming so we're gonna be lazy for now */
  GLuint fragmentShader, vertexShader, shaderProgram;
  AC3DObjectFactory factory;
  AC3DObject* model;

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

  //draws a model
  void modelRedraw(AC3DObject* node, Resources* r);

  void modelRedrawWBuf(AC3DObject* node, Resources* r);

private:
  double time;

  void modelRedrawLeaf(AC3DObject* obj);
  void modelRedrawTraverseWBuf(AC3DObject* node, Resources* r, int* verticesUsed);
  void modelRedrawLeafWBuf(AC3DObject* obj, Resources* r, int* verticesUsed);
};

/* Important global variables */
int screenWidth, screenHeight, isRunning;
Resources *resources;
World *world;

//Global variables for indexing when adding to the buffers
static int vertexBufIndex = 0, triIndicesBufIndex = 0, quadIndicesBufIndex = 0;

Resources::Resources() {
  /* Create shader and program objects, attach shaders to program */
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  factory = AC3DObjectFactory();

  vertexBuffer = nullptr; 
  triIndiceBuffer = nullptr;
  quadIndiceBuffer = nullptr;

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

  model = factory.loadAC3D("fighter.ac");

  //Create buffer arrays on CPU and generate and bind buffers on GPU
  buildBuffers(model);

  GLuint gpuBuffer[2];
  glGenBuffers(2, gpuBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, gpuBuffer[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*getVertexBufSize(), (void*)vertexBuffer, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuBuffer[1]);

  isOk=1;
}

void Resources::calcArraySizes()
{
	sizeOfVertexBuffer = sizeOfTriIndiceBuffer = sizeOfQuadIndiceBuffer = 0;
	traverseAndCount(model);
}

void Resources::traverseAndCount(AC3DObject* node)
{
	if (node->nChildren > 0)
	{
		for (int i = 0; i < node->nChildren; i++)
			traverseAndCount(node->children[i]);
	}
	else
	{		
		//*2 so that normals can be stored interleaved
		sizeOfVertexBuffer += node->nVertices * 2; 
		for (int i = 0; i < node->nSurfaces; i++)
		{
			if (node->surfaces[i].nVertices == 3)
				sizeOfTriIndiceBuffer++;
			else //if (node->surfaces[i].nVertices == 4)
				sizeOfQuadIndiceBuffer++;
		}
	}
}


void Resources::modelTraversal(AC3DObject* node)
{
	if (node->nChildren > 0)
	{
		for (int i = 0; i < node->nChildren; i++)
			modelTraversal(node->children[i]);
	}
	else
		modelLeaf(node);
}

void Resources::modelLeaf(AC3DObject* obj)
{
	//Create indices
	for (int i = 0; i < obj->nSurfaces; i++)
	{
		AC3DSurface surface = obj->surfaces[i];
		if (surface.nVertices == 3)
		{
			//Add indices to array using vertexBufIndex as offset to account for
			//vertices from previous objects preceding this objects vertices in
			//the buffer. vertexBufIndex is divided by 2 since each vertex is
			//stored together with its normal in the buffer.
			for (int j = 0; j < 3; j++)
				triIndiceBuffer[triIndicesBufIndex][j] = surface.vert[j].index + vertexBufIndex / 2;
			triIndicesBufIndex++;
		}
		else //if (obj->surfaces->nVertices == 4)
		{
			//Add indices to array using vertexBufIndex as offset to account for
			//vertices from previous objects preceding this objects vertices in
			//the buffer. vertexBufIndex is divided by 2 since each vertex is
			//stored together with its normal in the buffer.
			for (int j = 0; j < 4; j++)
				quadIndiceBuffer[quadIndicesBufIndex][j] = surface.vert[j].index + vertexBufIndex / 2; 
			quadIndicesBufIndex++;
		}
	}
	
	glm::vec4* normals = new glm::vec4[obj->nVertices];
	//Zero all normals
	for (int i = 0; i < obj->nVertices; i++)
		normals[i] = glm::vec4(0);

	//Create normals
	glm::vec4* surface_normals = new glm::vec4[obj->nSurfaces];
	for (int i = 0; i < obj->nSurfaces; i++)
	{
		glm::vec3 vertices[3];

		for (int j = 0; j < 3; j++)
			vertices[j] = glm::vec3(obj->vertices[obj->surfaces[i].vert[j].index * 4],
									obj->vertices[obj->surfaces[i].vert[j].index * 4 + 1],
									obj->vertices[obj->surfaces[i].vert[j].index * 4 + 2]);

		glm::vec3 normal = glm::cross(glm::normalize(vertices[1] - vertices[0]), glm::normalize(vertices[2] - vertices[1]));

		surface_normals[i].x = normal.x;
		surface_normals[i].y = normal.y;
		surface_normals[i].z = normal.z;
	}

	//Assign normals to vertices (proper method)
	for (int i = 0; i < obj->nSurfaces; i++)
	{
		for (int j = 0; j < obj->surfaces[i].nVertices; j++)
			normals[obj->surfaces[i].vert[j].index] += surface_normals[i];
	}
	for (int i = 0; i < obj->nVertices; i++)
		normals[i] = glm::normalize(normals[i]);

	//Add vertices and normals to buffer array
	for (int i = 0; i < obj->nVertices; i++)
	{
		//Construct vec4 from an array of floats
		vertexBuffer[vertexBufIndex++] = glm::vec4(obj->vertices[i * 4],
												   obj->vertices[i * 4 + 1],
												   obj->vertices[i * 4 + 2],
												   obj->vertices[i * 4 + 3]);
		vertexBuffer[vertexBufIndex++] = normals[i];
	}

	delete normals;
	delete surface_normals;
	//delete indices;
}

void Resources::buildBuffers(AC3DObject* node)
{
	calcArraySizes();

	if (vertexBuffer != nullptr)
		delete vertexBuffer;
	vertexBuffer = new glm::vec4[sizeOfVertexBuffer];

	if (triIndiceBuffer != nullptr)
		delete triIndiceBuffer;
	triIndiceBuffer = new glm::ivec3[sizeOfTriIndiceBuffer];

	if (quadIndiceBuffer != nullptr)
		delete quadIndiceBuffer;
	quadIndiceBuffer = new glm::ivec4[sizeOfQuadIndiceBuffer];

	vertexBufIndex = 0;
	triIndicesBufIndex = 0;
	quadIndicesBufIndex = 0;

	modelTraversal(node);
}

World::World() {
  time=0.0;
}

void World::modelRedrawWBuf(AC3DObject* node, Resources* r)
{
	//Setup attribPointers
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2, (void*) &r->vertexBuffer[0]);
	//glEnableVertexAttribArray(0);

	//glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2, (void*) &r->vertexBuffer[1]);
	//glEnableVertexAttribArray(2);

	//int verticesUsed = 0;
	//modelRedrawTraverseWBuf(node, r, &verticesUsed);

	//Setup attribPointers with vertexBuffer
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2, nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2, (void*)sizeof(glm::vec4));
	glEnableVertexAttribArray(2);

	//Upload triangle indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3)*r->getTriIndicesBufSize(), (void*)r->triIndiceBuffer, GL_STATIC_DRAW);

	glDrawElements(GL_TRIANGLES, 3 * r->getTriIndicesBufSize(), GL_UNSIGNED_INT, nullptr);

	//Upload quad indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec4)*r->getQuadIndicesBufSize(), (void*)r->quadIndiceBuffer, GL_STATIC_DRAW);

	glDrawElements(GL_QUADS, 4 * r->getQuadIndicesBufSize(), GL_UNSIGNED_INT, nullptr);
}

void World::modelRedrawTraverseWBuf(AC3DObject* node, Resources* r, int* verticesUsed)
{
	if (node->nChildren > 0)
	{
		for (int i = 0; i < node->nChildren; i++)
			modelRedrawTraverseWBuf(node->children[i], r, verticesUsed);
	}
	else
		modelRedrawLeafWBuf(node, r, verticesUsed);
}

void World::modelRedrawLeafWBuf(AC3DObject* obj, Resources* r, int* verticesUsed)
{
	//Old code before index buffers were added
	//glm::ivec4* indices = new glm::ivec4[obj->nSurfaces];

	////Create indices
	//for (int i = 0; i < obj->nSurfaces; i++)
	//{
	//	AC3DSurface surface = obj->surfaces[i];
	//	for (int j = 0; j < surface.nVertices; j++)
	//		indices[i][j] = surface.vert[j].index + *verticesUsed;
	//}

	//*verticesUsed += obj->nVertices;

	////Draw
	//for (int i = 0; i < obj->nSurfaces; i++)
	//{
	//	if (obj->surfaces[i].nVertices == 3)
	//		glDrawElements(GL_TRIANGLES, 1 * 3, GL_UNSIGNED_INT, &indices[i][0]);
	//	else //(obj->surfaces[i].nVertices == 4)
	//		glDrawElements(GL_QUADS, 1 * 4, GL_UNSIGNED_INT, &indices[i][0]);
	//}

	//delete indices;
}

void World::modelRedraw(AC3DObject* node, Resources* r)
{
	if (node->nChildren > 0)
	{
		for (int i = 0; i < node->nChildren; i++)
			modelRedraw(node->children[i], r);
	}
	else
		modelRedrawLeaf(node);
}


void World::modelRedrawLeaf(AC3DObject* obj)
{
	glm::ivec4* indices = new glm::ivec4[obj->nSurfaces];

	//Create indices
	for (int i = 0; i < obj->nSurfaces; i++)
	{
		AC3DSurface surface = obj->surfaces[i];
		for (int j = 0; j < surface.nVertices; j++)
			indices[i][j] = surface.vert[j].index;
	}

	glm::vec4* normals = new glm::vec4[obj->nVertices];
	//Zero all normals
	for (int i = 0; i < obj->nVertices; i++)
		normals[i] = glm::vec4(0);

	//Create normals
	glm::vec4* surface_normals = new glm::vec4[obj->nSurfaces];
	for (int i = 0; i < obj->nSurfaces; i++)
	{
		glm::vec3 vertices[3];

		//"stitch" together the 3 vertex values to a vec3
		for (int j = 0; j < 3; j++)
			vertices[j] = glm::vec3(obj->vertices[obj->surfaces[i].vert[j].index * 4],
									obj->vertices[obj->surfaces[i].vert[j].index * 4 + 1],
									obj->vertices[obj->surfaces[i].vert[j].index * 4 + 2]);
									
		//Calculate new vector perpendicular to the plane made by the first three vertices
		glm::vec3 normal = glm::cross(glm::normalize(vertices[1] - vertices[0]), glm::normalize(vertices[2] - vertices[1]));

		surface_normals[i].x = normal.x;
		surface_normals[i].y = normal.y;
		surface_normals[i].z = normal.z;
		surface_normals[i].w = 0;
	}

	//Assign normals to vertices (hack method)
	//for (int i = 0; i < obj->nSurfaces; i++)
	//{
	//	for (int j = 0; j < obj->surfaces[i].nVertices; j++)
	//		normals[obj->surfaces[i].vert[j].index] = surface_normals[i];
	//}

	//Assign normals to vertices (proper method)
	for (int i = 0; i < obj->nSurfaces; i++)
	{
		for (int j = 0; j < obj->surfaces[i].nVertices; j++)
			normals[obj->surfaces[i].vert[j].index] += surface_normals[i];
	}
	for (int i = 0; i < obj->nVertices; i++)
		normals[i] = glm::normalize(normals[i]);

	//Setup attribPointers
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*) obj->vertices);  
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*) normals);
	glEnableVertexAttribArray(2);

	//Draw
	for (int i = 0; i < obj->nSurfaces; i++)
	{
		if (obj->surfaces[i].nVertices == 3)
			glDrawElements(GL_TRIANGLES, 1 * 3, GL_UNSIGNED_INT, &indices[i][0]);
		else //if (obj->surfaces[i].nVertices == 4)
			glDrawElements(GL_QUADS, 1 * 4, GL_UNSIGNED_INT, &indices[i][0]);
	}

	delete indices;
	delete normals;
	delete surface_normals;
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

  const GLfloat n = 1, f = 10;
  const GLfloat a = (f + n) / (f - n);
  const GLfloat b = 2 * f*n / (f - n);

  // projectionMatrix
  const GLfloat projectionMatrix[4][4] = {
	  { 1.0, 0.0, 0.0, 0.0 },
	  { 0.0, 1.0, 0.0, 0.0 },
	  { 0.0, 0.0,  -a,  -b },
	  { 0.0, 0.0,  -1, 0.0 } };

  // modelviewatrix
  glm::mat4 modelviewMatrix =
	  glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -2)) *
	  glm::rotate(glm::mat4(1.0f), (float)time, glm::vec3(0, 1, 0)) * //*(float)time
	  glm::rotate(glm::mat4(1.0f), 0.4f*(float)time, glm::vec3(0, 0, 1));

  // light position
  const glm::vec4 light0pos = { 2, 1, 0, 1 };

  glUseProgram(r->shaderProgram);

  //Pass over matrices
  glUniformMatrix4fv(glGetUniformLocation(r->shaderProgram, "projectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(r->shaderProgram, "modelviewMatrix"), 1, GL_FALSE, &modelviewMatrix[0][0]);

  glUniform4fv(glGetUniformLocation(r->shaderProgram, "light0pos"), 1, &light0pos[0]);

  //Draw model
  //modelRedraw(r->model, r);
  modelRedrawWBuf(r->model, r);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  if((error=glGetError()) != GL_NO_ERROR) {
    fprintf(stderr,"GL Error: %s\n",gluErrorString(error));
  }
}

void World::doKeyboard(int key,int mouseX,int mouseY) {
  /** TODO - implement a keyhandler that does something interesting when given inputs */
  switch(key) {
  }
}

void World::tick(double deltaTime) { time += deltaTime; }


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
}

#ifdef WINDOWS
#include "stdafx.h"
int _tmain(int argc, _TCHAR* argv[]) {
  main(0,NULL);
  return 0;
}
#endif
