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

// ser_pair.h

#ifndef INCLUDED_SER_PAIR_H
#define INCLUDED_SER_PAIR_H

#include "serdefs.h"
#include "serializer.h"

#include <utility>

/**
 * SerializeHelper functions for pair<T1,T2>
 **/

namespace SerializeHelper
{
  template <typename T1, typename T2>
  inline SerializeTag ComputeTag(const std::pair<T1,T2>&)
  {
    return eTagPair;
  }

  template <typename T1, typename T2>
  inline bool PutInto(Serializer& ser, const std::pair<T1,T2>& obj)
  {
    return ser.Put(obj.first) && ser.Put(obj.second);
  }

  template <typename T1, typename T2>
  inline bool GetFrom(Serializer& ser, std::pair<T1,T2>& obj)
  {
    return ser.Get(obj.first) && ser.Get(obj.second);
  }
};

#endif // !INCLUDED_SER_PAIR_H
