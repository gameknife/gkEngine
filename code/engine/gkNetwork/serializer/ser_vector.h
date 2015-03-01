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

// ser_vector.h

#ifndef INCLUDED_SER_VECTOR_H
#define INCLUDED_SER_VECTOR_H

#include "serdefs.h"
#include "ser_stl.h"

#include <vector>

/**
 * SerializeHelper functions for vector<>
 **/

namespace SerializeHelper
{
  template <typename T>
  inline SerializeTag ComputeTag(const std::vector<T>&)
  {
    return eTagVector;
  }

  template <typename T>
  inline bool PutInto(Serializer& ser, const std::vector<T>& obj)
  {
    return PutRange(ser, obj.begin(), obj.end(), obj.size());
  }

  template <typename T>
  inline bool GetFrom(Serializer& ser, std::vector<T>& obj)
  {
    obj.clear();
    return GetRange(ser, std::back_inserter(obj), (T*)0);
  }
}

#endif // !INCLUDED_SER_VECTOR_H
