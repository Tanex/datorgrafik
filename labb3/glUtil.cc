/* shaders.cc
   Routines for loading a vertex+fragment shader pair

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,we
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "general.h"
#include "glm/glm.hpp"
#include "glUtil.h"

int compileShader(GLuint shader, const char *shaderSourceName, const char *logOutputName) {
  FILE *fp;
  FILE *logFile;
  GLchar *shaderSource;
  GLchar *sources[2];
  char logName[256];
  int i,line, size;

  printf("loading '%s'\n",shaderSourceName);

  /* Attempt to open the source file */
  fp=fopen(shaderSourceName,"rb");
  if(!fp) { 
    fprintf(stderr,"ERROR: Failed to load shader '%s'\n",shaderSourceName); 
    return -1; 
  }
  if(logOutputName) {
    logFile=fopen(logOutputName,"wb");
    if(!logFile) {
      fprintf(stderr,"WARNING: could not open '%s' for output\n",logOutputName);
    }
  } else logFile=NULL;

  /* Get the length of sourcecode file and allocate buffer*/
  fseek(fp, 0, SEEK_END);
  size=ftell(fp) + 1;
  fseek(fp, 0, SEEK_SET);
  shaderSource=(char*)malloc(size);

  /* Load the shader code from disk and add null-termination */
  int l=fread((void*)shaderSource,sizeof(char),size,fp);
  if(l < size-1) {
    fprintf(stderr,"ERROR - incorrect length while load shader sources from '%s'. Got %d, expected %d\n",shaderSourceName,l,size);
  }
  shaderSource[size-1]=0;

  /* Check plausibility of sourcecode file - mostly to help you detect
     illegal invicible characters in the file (!) */
  for(i=0,line=0;i<size;i++) {
    if(!shaderSource[i]) break;
    if(!(isspace(shaderSource[i]) || isgraph(shaderSource[i]) ))  {
      fprintf(stderr,"Illegal char %d '%c' in %s@%d\n",shaderSource[i],shaderSource[i],shaderSourceName,line);
      fclose(fp);
      if(logFile) fclose(logFile);
      return -1;
    }
    if(shaderSource[i] == '\n') line++;
  }

  /* Pass the sourcecode to the shader object */
  sources[0]=shaderSource;
  glShaderSource(shader, 1, (const GLchar**) sources, NULL);
  glCompileShader(shader);

  GLint compileStatus, len;
  GLchar infoLog[65536];

  /* Check status of compilation, generate outputs and possible
     warning message */
  glGetShaderiv(shader,GL_COMPILE_STATUS,&compileStatus);
  printf("Compile status: %d\n",compileStatus);
  glGetShaderInfoLog(shader,sizeof(infoLog)-1,&len,infoLog);
  if(logFile) fwrite(infoLog, sizeof(char), strlen(infoLog), logFile);
  if(compileStatus != GL_TRUE) {
    printf("Shader '%s' compilation failed\n%s",shaderSourceName,infoLog);    
    fclose(fp);
    if(logFile) fclose(logFile);
    return -1;
  } 
  printf("Shader '%s' compiled succesfully\n",shaderSourceName);

  fclose(fp);
  fclose(logFile);
  return 0;
}
int linkProgram(GLuint program, const char *logOutputName) {
  GLint progLinkSuccess, len;
  GLchar infoLog[65536];
  FILE *logFile;

  if(logOutputName) {
    logFile=fopen(logOutputName,"wb");
    if(!logFile) {
      fprintf(stderr,"WARNING: could not open '%s' for output\n",logOutputName);
    }
  } else logFile=NULL;

  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &progLinkSuccess);
  glGetProgramInfoLog(program,65536,&len,infoLog);
  if(logFile) fwrite(infoLog, sizeof(char), strlen(infoLog), logFile);
  if (!progLinkSuccess) {
    fprintf(stderr, "Program %d could not be linked:\n%s\n",program,infoLog);
    if(logFile) fclose(logFile);
    return -1;
  }
  if(logFile) fclose(logFile);
  return 0;
}


int loadTexture(GLuint texture, const char *filename, int generateMipmaps) {  
  SDL_Surface *surface, *image;
  SDL_Rect area;
  int w,h;

  surface = IMG_Load(filename);
  if(!surface) { 
    printf("Warning, failed to load '%s'\n",filename); 
    return -1;
  }

  w=surface->w;
  h=surface->h;

  image = SDL_CreateRGBSurface(
			       SDL_SWSURFACE, w,h, 32,
				  
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
				  0x000000FF, 
				  0x0000FF00, 
				  0x00FF0000, 
				  0xFF000000
#else
				  0xFF000000,
				  0x00FF0000, 
				  0x0000FF00, 
				  0x000000FF
#endif
				  );
  
  if(image == NULL) {
    SDL_FreeSurface(surface);
    printf("Warning, failed to allocate temporary image\n");
    return -1;
  }

  SDL_SetAlpha(surface, 0, 0);
  /* Copy the surface into the GL texture image */
  area.x = 0;
  area.y = 0;
  area.w = w;
  area.h = h;
  SDL_BlitSurface(surface, NULL, image, &area);

  /* Create an OpenGL texture for the image */       
  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);

  /*int x,y;
  for(x=0;x<w;x++)
    for(y=0;y<h;y++) {
      ((char*) image->pixels)[(x+y*w)*4+0] = 127;
      ((char*) image->pixels)[(x+y*w)*4+1] = x;
      ((char*) image->pixels)[(x+y*w)*4+2] = y;
      }*/


  printf("loading the texture data...\n");

  if(generateMipmaps)
    gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,w,h,GL_RGBA,GL_UNSIGNED_BYTE,image->pixels);
  else
    glTexImage2D(GL_TEXTURE_2D,
		 0,
		 GL_RGBA,w,h,0,
		 GL_RGBA,
		 GL_UNSIGNED_BYTE,
		 image->pixels);	
  
  //SDL_FreeSurface(image);
  SDL_FreeSurface(surface);

  return 0;
}

AC3DObject::AC3DObject() {}
void AC3DObject::finalize() {}
AC3DObjectFactory::AC3DObjectFactory() :nMaterials(0), allocatedMaterials(0), materials(NULL), currentDepth(0) {}
AC3DObjectFactory::~AC3DObjectFactory() {
  if(materials) { delete materials; }
}
AC3DObject *AC3DObjectFactory::makeObject() { return new AC3DObject(); }
AC3DObject *AC3DObjectFactory::loadAC3D(const char *filename) {
  FILE *fp;
  char str[256];
  fp = fopen(filename,"rb");
  if(!fp) {
    fprintf(stderr,"Error - could not open '%s'\n",filename);
    return NULL;
  }
  /* Could not read header */
  if(fread(str,1,5,fp) != 5) return NULL;
  /* Invalid header */
  if(strncmp(str,"AC3D",4) != 0) return NULL;
  int version = str[4];
  printf("Parsing AC3D file '%s' @version: %d\n",filename,version);
  /* Flush to first line after header */
  while(fgetc(fp) != '\n') { if(feof(fp)) return NULL; }
  
  /* Reset all materials */
  if(materials) delete materials; 
  materials=NULL;
  nMaterials=0;
  allocatedMaterials=0;

  AC3DObject *root = parseObject(fp);

  if(materials) delete materials;
  materials=NULL;
  nMaterials=0;
  allocatedMaterials=0;

  return root;
}
AC3DObject *AC3DObjectFactory::parseObject(FILE *fp) {
  char str[256];
  int i;

  AC3DObject *object = makeObject();

  while(1) {
    str[0]=0;
    fscanf(fp,"%32s",str);
    /* Scan past any material definitions until the next object is found */
    if(strncmp(str,"OBJECT",32) == 0) break;
    else if(strncmp(str,"MATERIAL",32) == 0) {
      /* make sure we have enough memory to store this material objects */
      if(allocatedMaterials < nMaterials+1) {
	if(allocatedMaterials == 0) {
	  allocatedMaterials=32;
	  materials=new AC3DMaterial[32];
	} else {
	  allocatedMaterials*=2;
	  AC3DMaterial *newMat = new AC3DMaterial[allocatedMaterials];
	  memcpy((void*) newMat, materials, sizeof(AC3DMaterial)*nMaterials);
	  delete materials;
	  materials=newMat;
	}
      }
      /* Read name of material */
      fscanf(fp,"%32s",materials[nMaterials].name);
      if(fscanf(fp," rgb %f %f %f",&materials[nMaterials].rgb[0],&materials[nMaterials].rgb[1],&materials[nMaterials].rgb[2]) != 3) 
	 fprintf(stderr,"Parse error in AC3D file\n");
      if(fscanf(fp," amb %f %f %f",&materials[nMaterials].amb[0],&materials[nMaterials].amb[1],&materials[nMaterials].amb[2]) != 3)
	fprintf(stderr,"Parse error in AC3D file\n");
      if(fscanf(fp," emis %f %f %f",&materials[nMaterials].emis[0],&materials[nMaterials].emis[1],&materials[nMaterials].emis[2]) != 3)
	fprintf(stderr,"Parse error in AC3D file\n");
      if(fscanf(fp," spec %f %f %f",&materials[nMaterials].spec[0],&materials[nMaterials].spec[1],&materials[nMaterials].spec[2]) != 3)
	fprintf(stderr,"Parse error in AC3D file\n");
      if(fscanf(fp," shi %d",&i) != 1)
	fprintf(stderr,"Parse error in AC3D file\n");
      materials[nMaterials].shi = i;
      if(fscanf(fp," trans %f",&materials[nMaterials].trans) != 1)
	fprintf(stderr,"Parse error in AC3D file\n");
      printf("Material %s parsed\n",materials[nMaterials].name);
      nMaterials++;
    } else {
      fprintf(stderr,"Parse error in AC3D file (unknown top-level tag %s)\n",str);
      return NULL;
    }
  }
  /* Default values for rotation/translation matrix */  
  object->mat=glm::mat4(1);  
  object->url[0]=0;
  object->name[0]=0;
  object->type[0]=0;
  object->textureName[0]=0;
  object->textureRepeat[0]=1.0;
  object->textureRepeat[1]=1.0;
  object->nVertices=0;
  object->nSurfaces=0;
  object->nTriangles=0;
  object->nQuads=0;

  /* Definition of this object found - parse type */
  if(fscanf(fp,"%31s",object->type) != 1) {
    fprintf(stderr,"Parse error in AC3D file, could not read type\n");
    return NULL;
  }  
  /* Parse each of the contents, stop after finding the kids */  
  while(1) {
    if(feof(fp)) {
      fprintf(stderr,"Parse error in AC3D file, EOF before end of object\n");
      return NULL;
    }
    if(fscanf(fp,"%32s",str) != 1) {
      fprintf(stderr,"Parse error in AC3D file, could not read next tag\n");
      return NULL;
    }
    else if(strncmp(str,"name",32) == 0) {
      fscanf(fp,"%127s",object->name); 
    }
    else if(strncmp(str,"data",32) == 0) {
      /* Skip the data section of the object. The standard does not say anything about what it contains */
      if(fscanf(fp,"%d",&i) != 1)  { printf("Parse error in AC3D file, could not skip data tag\n"); return NULL; }
      while(fgetc(fp) != '\n') if(feof(fp)) { printf("Parse error in AC3D file, error skipping data tag\n"); return NULL; }
      for(;i>0;i--) { fgetc(fp); if(feof(fp)) { printf("Parse error in AC3D file, error skipping data tag\n"); return NULL; }}
    }
    else if(strncmp(str,"texture",32) == 0) {
      fscanf(fp,"%128s",object->textureName);     
    }
    else if(strncmp(str,"texrep",32) == 0) {
      if(fscanf(fp,"%f %f",&object->textureRepeat[0], &object->textureRepeat[1]) != 2)  { printf("Parse error in AC3D file, texture repeat\n"); return NULL; }
    }
    else if(strncmp(str,"rot",32) == 0) {
      if(fscanf(fp,"%f %f %f %f %f %f %f %f %f",
		&object->mat[0][0],&object->mat[0][1],&object->mat[0][2],
		&object->mat[1][0],&object->mat[1][1],&object->mat[1][2],
		&object->mat[2][0],&object->mat[2][1],&object->mat[2][2]) != 9) {
	printf("Parse error in AC3D file, rotation matrix\n"); return NULL; 
      }
    }
    else if(strncmp(str,"loc",32) == 0) {
      if(fscanf(fp,"%f %f %f",&object->mat[3][0],&object->mat[3][1],&object->mat[3][2]) != 3) {
	printf("Parse error in AC3D file, location matrix\n"); return NULL; 
      }   
    }
    else if(strncmp(str,"url",32) == 0) {
      fscanf(fp,"%127s",object->url); 
    }
    else if(strncmp(str,"numvert",32) == 0) {
      if(fscanf(fp,"%d",&i) != 1) { printf("Parse error in AC3D file, numvertices\n"); return NULL; }
      object->nVertices=i;
      object->vertices=new GLfloat[object->nVertices*4];
      for(i=0;i<object->nVertices;i++) {
	if(fscanf(fp," %f %f %f",object->vertices+i*4+0,object->vertices+i*4+1,object->vertices+i*4+2) != 3) { 
	  printf("Parse error in AC3D file,numvertices (2)\n"); return NULL; 
	}
	object->vertices[i*4+0] *= 0.1;
	object->vertices[i*4+1] *= 0.1;
	object->vertices[i*4+2] *= 0.1;
	object->vertices[i*4+3]=1.0;
      }
    }
    else if(strncmp(str,"numsurf",32) == 0) {
      if(fscanf(fp," %d",&i) != 1) { printf("Parse error in AC3D file, numsurf\n"); return NULL; }
      object->nSurfaces=i;
      object->surfaces = new AC3DSurface[object->nSurfaces];
      for(int surf=0;surf<object->nSurfaces;surf++) {
	AC3DSurface *surface=&object->surfaces[surf];
	fscanf(fp," %32s",str);
	if(strncmp(str,"SURF",32) != 0) {
	  printf("Parse error in AC3D file, expected another surface (got %s)\n",str); return NULL; 	  
	}
	if(fscanf(fp," %x",&i) != 1) { printf("Parse error in AC3D file, expected surface type\n"); return NULL; }		
	surface->flags=i;
	if(fscanf(fp," mat %d",&i) != 1) { printf("Parse error in AC3D file, expected mat\n"); return NULL; }		
	surface->material=i;
	if(fscanf(fp," refs %d",&i) != 1) { printf("Parse error in AC3D file, expected refs\n"); return NULL; }
	surface->nVertices=i;
	if(surface->nVertices == 3) object->nTriangles++;
	else if(surface->nVertices == 4) object->nQuads++;
	if(surface->nVertices > 4) { printf("Too many vertices (%d) in surface inside AC3D file (max 4 supported)\n",surface->nVertices); return NULL; }	
	for(i=0;i<surface->nVertices;i++) {
	  if(fscanf(fp," %d %f %f",&surface->vert[i].index,&surface->vert[i].uv[0],&surface->vert[i].uv[1]) != 3) { 
	    printf("Parse error in AC3D file, could not read vertice\n"); return NULL; 
	  }
	}
      }
    }
    else if(strncmp(str,"kids",32) == 0) {
      if(fscanf(fp,"%d",&i) != 1) { printf("Parse error in AC3D file, no kids\n"); return NULL; }
      object->nChildren=i;
      break;
    }
  }
  for(i=0;i<currentDepth;i++) printf("   ");
  printf("OBJECT %s (%s) %d vertices, %d surfaces (%d+%d)\n",object->name,object->type,object->nVertices,object->nSurfaces,object->nTriangles,object->nQuads);
  currentDepth++;
  /* Parse any children */
  if(object->nChildren) {
    object->children = (AC3DObject**) new AC3DObject*[object->nChildren];
    for(i=0;i<object->nChildren;i++) {
      object->children[i] = parseObject(fp);
      if(!object->children[i]) return NULL;
    }
  } else object->children=NULL;
  object->finalize();
  currentDepth--;
  return object;
}
