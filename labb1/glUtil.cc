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
  glBindTexture(GL_TEXTURE_2D, texture);

  if(generateMipmaps)
    gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,w,h,GL_RGBA,GL_UNSIGNED_BYTE,image->pixels);
  else
    glTexImage2D(GL_TEXTURE_2D,
		 0,
		 GL_RGBA,w,h,0,
		 GL_RGBA,
		 GL_UNSIGNED_BYTE,
		 image->pixels);	
  
  SDL_FreeSurface(image);
  SDL_FreeSurface(surface);

  return 0;
}

