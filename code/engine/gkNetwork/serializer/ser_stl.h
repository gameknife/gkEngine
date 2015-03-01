//////////////////////////////////////////////////////////////////////////
/*
Copyright (c) 2011-2015 Kaiming Yi
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
	
*/
//////////////////////////////////////////////////////////////////////////



// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// ser_stl.h

#ifndef INCLUDED_SER_STL_H
#define INCLUDED_SER_STL_H

#include "serializer.h"

/**
 * SerializeHelper functions for STL containers
 **/

namespace SerializeHelper
{
  /**
   * Helper for STL PutInto functions
   **/

  template <typename Iterator>
  bool PutRange(Serializer& ser, const Iterator& begin, const Iterator& end, uint32 numItems)
  {
    // store the size
    if (!ser.PutCount(numItems))
      return false;
    // store each element
    uint32 count = 0;
    for (Iterator it = begin; it != end; ++it, ++count)
      if (!ser.Put(*it))
        return false;
    // just in case distance(begin,end) != numItems
    return (count == numItems);
  }

  /**
   * Helper for STL GetFrom functions
   **/

  template <typename T, typename Iterator>
  bool GetRange(Serializer& ser, Iterator it, const T*)
  {
    // read size
    uint32 size;
    if (!ser.GetCount(size))
      return false;
    // read each item
    for (uint32 i = 0; i < size; ++i)
    {
      T obj;
      if (!ser.Get(obj))
        return false;
      *it = obj; // insertion
    }
    return true;
  }

}

#endif // !INCLUDED_SER_STL_H
