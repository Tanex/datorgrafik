/** \file camera.h
    \brief Declares all methods for the Camera class. 
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

#ifndef   	CAMERA_H_
# define   	CAMERA_H_

/** \brief Represents the camera and determines the point from which rays are
    cast in the scene. 

    Use this to modify the perspective and to move
    the camera between frames. */
class Camera {
 public:
  Camera();
  void setOrigin(double[3]);

  /** \brief Defines the up vector used by the camera, usually set to
      +Y. 
      
      Must be orthogonal to right and forward vectors. */
  void setUp(double[3]);
  /** \brief Defines the right vector used by the camera. 

      Must be orthogonal to up and forward vectors. */
  void setRight(double[3]);
  /** \brief Defines the forward vector used by the camera. 

      Must be orthogonal to right and up vectors. */
  void setForward(double[3]);

  /** Computes up/right/forward vectors so that we are focusing on
      given point. Uses old values of up as hint whenever possible. */
  void setFocus(double[3]);

  /** Assigns the origin/direction ray corresponding to a given pixel
      where x/y is a fraction 0.0 - 1.0 of screen width/height */
  void getPixelRay(double x,double y,double origin[3],double direction[3]);
 private:
  double origin[3];

  double up[3];
  double right[3];
  double forward[3];

  double tanFovY,tanFovX;
};

#endif 	    /* !CAMERA_H_ */
