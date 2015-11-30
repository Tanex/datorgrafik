/** \file referenced.h
    \brief Implements reference counters for all objects used by the
    raytracer via the ReferencedObject class.
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

#ifndef   	REFERENCED_H_
# define   	REFERENCED_H_

/** \brief Base class for all objects which use a reference counter
    for memory management. */
class ReferencedObject {
 public:
  ReferencedObject();
  virtual ~ReferencedObject();

  /** Call when an object reference is added to memory. Increases the reference counter by one */
  void reference();

  /** Call when an object is dereferenced. Decreases the reference
      counter by one and deallocates the memory for the objectif neccessary. */  
  void dereference();
 private:
  int referenceCounter;
};

#endif 	    /* !REFERENCED_H_ */
