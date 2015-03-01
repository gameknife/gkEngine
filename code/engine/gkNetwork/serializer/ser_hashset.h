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

// ser_hashset.h

#ifndef INCLUDED_SER_HASHSET_H
#define INCLUDED_SER_HASHSET_H

#include "serdefs.h"
#include "ser_stl.h"

// sheesh, who was it that decided header files without ".h"
// was a good idea anyway?

#if defined(COMPILER_MSVC)
# include <hash_set>
#else
# include <hash_set.h>
#endif

/**
 * SerializeHelper functions for hash_set<> and hash_multiset<>
 **/

namespace SerializeHelper
{
  template <typename T, typename HASH, typename EQUAL>
  inline SerializeTag ComputeTag(const std::hash_set<T,HASH,EQUAL>&)
  {
    return eTagHashSet;
  }

  template <typename T, typename HASH, typename EQUAL>
  inline bool PutInto(Serializer& ser, const std::hash_set<T,HASH,EQUAL>& obj)
  {
    return PutRange(ser, obj.begin(), obj.end(), obj.size());
  }

  template <typename T, typename HASH, typename EQUAL>
  inline bool GetFrom(Serializer& ser, std::hash_set<T,HASH,EQUAL>& obj)
  {
    obj.clear();
    return GetRange(ser, std::inserter(obj, obj.end()), (T*)0);
  }

  template <typename T, typename HASH, typename EQUAL>
  inline SerializeTag ComputeTag(const std::hash_multiset<T,HASH,EQUAL>&)
  {
    return eTagHashMultiSet;
  }

  template <typename T, typename HASH, typename EQUAL>
  inline bool PutInto(Serializer& ser, const std::hash_multiset<T,HASH,EQUAL>& obj)
  {
    return PutRange(ser, obj.begin(), obj.end(), obj.size());
  }

  template <typename T, typename HASH, typename EQUAL>
  inline bool GetFrom(Serializer& ser, std::hash_multiset<T,HASH,EQUAL>& obj)
  {
    obj.clear();
    return GetRange(ser, std::inserter(obj, obj.end()), (T*)0);
  }
}

#endif // !INCLUDED_SER_HASHSET_H
