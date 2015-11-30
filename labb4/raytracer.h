/** \file raytracer.h
    \brief Declares the main raytracing functionality for the
    Raytracer class. 
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

#ifndef   	RAYTRACER_H_
# define   	RAYTRACER_H_

#ifndef CAMERA_H_
#include "camera.h"
#endif

#ifndef OBJECT_H_
#include "object.h"
#endif

#ifndef LIGHT_H_
#include "light.h"
#endif

/** \brief Main class for performing all raytracing operations. 

    To use, instantiate this class and give it a scene graph using the
    addObject and addLight operations. Also pass it a camera object
    using the setCamera function and give global lighting properties
    using setBackground nad setAmbientLight. 

    Once ready to use, the actual raytracing operation can be
    performed by calling the raytracer function for every pixel on the
    screen. 

    Note that all objects are asssumed to be reentrant during the
    raytracring (ie. they should not change any internal state
    variables), and that updates on them should only be made in
    between frames. 
*/
class Raytracer {
 public:
  Raytracer();
  ~Raytracer();

  /** Sets the background colour of the scene, ie. the colour used
      for all pixels that hit no object. */
  void setBackground(double[3]);

  /** Sets the global ambient light in the scene. */
  void setAmbientLight(double[3]);

  /** \brief Adds a lightsource to the scene. */
  void addLight(Light *);
  /** \brief Adds an object to the scene. */
  void addObject(Object *);
  /** \brief Sets the camera object to be used when determining origin
      of the rays. */
  void setCamera(Camera *);
  /** \brief Gives the camera object currently used. */
  Camera *getCamera();

  /** Basic raytracing routine that computes a colour to assign to a
  ray originating in origin with the given direction. If no
  intersecting objects are found then it assigns the background colour
  to the vector rgb, otherwise the result of doing the light
  computations and recursive raytracing from the intersection point. 
  
  Contribution is a hint for how much the resuling colours will
  contribute to the screen pixels and can be used to limit recursion. 
  */
  void raytrace(double origin[3],double direction[3],double rgb[3],double contribution);
  /** Special case of general raytracing routine for screen pixel
      X,Y. Fetches the origin/direction from the current camera
      settings. */
  void raytrace(int x, int y,double rgb[3]);
 private:
  Camera *camera;
  double background[3];
  double ambientLight[3];

  class std::set<Light*> *lights;
  class std::set<Object*> *objects;
};

#endif 	    /* !RAYTRACER_H_ */
