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




// mtrand.h
// C++ include file for MT19937, with initialization improved 2002/1/26.
// Coded by Takuji Nishimura and Makoto Matsumoto.
// Ported to C++ by Jasper Bedaux 2003/1/1 (see http://www.bedaux.net/mtrand/).
// The generators returning floating point numbers are based on
// a version by Isaku Wada, 2002/01/09
// Static shared data converted to per-instance, 2008-11-13 by JSP @Crytek.
//
// Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. The names of its contributors may not be used to endorse or promote
//    products derived from this software without specific prior written
//    permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Any feedback is very welcome.
// http://www.math.keio.ac.jp/matumoto/emt.html
// email: matumoto@math.keio.ac.jp
//
// Feedback about the C++ port should be sent to Jasper Bedaux,
// see http://www.bedaux.net/mtrand/ for e-mail address and info.

//-------------------------------------------------------------------------
// History:
// - 28:7:2005: File created and minor changes by Marco Corbetta
//
//*************************************************************************/


#ifndef MTRAND_H
#define MTRAND_H

//////////////////////////////////////////////////////////////////////////
class CMTRand_int32 
{ 
	// Mersenne Twister random number generator

public:
	// default constructor
	CMTRand_int32() { seed(5489UL); }
	// constructor with 32 bit int as seed
	CMTRand_int32(uint32 s) { seed(s); }
	// constructor with array of size 32 bit ints as seed
	CMTRand_int32(const uint32* array, int size) { seed(array, size); }
	// the two seed functions
	void seed(uint32); // seed with 32 bit integer
	void seed(const uint32*, int size); // seed with array
	// overload operator() to make this a generator (functor)
	//uint32 operator()() { return rand_int32(); }
	uint32 Generate() { return rand_int32(); }

	// generates random floating number in the closed interval [0,1].
	float GenerateFloat() { return (float)rand_int32()*(1.0f/4294967295.0f); }

	~CMTRand_int32() {} // destructor

protected: // used by derived classes, otherwise not accessible; use the ()-operator
	//uint32 rand_int32(); // generate 32 bit random integer
	uint32 rand_int32() 
	{ // generate 32 bit random int
		if (p >= n) gen_state(); // new m_nState vector needed
		// gen_state() is split off to be non-inline, because it is only called once
		// in every 624 calls and otherwise irand() would become too big to get inlined
		uint32 x = m_nState[p++];
		x ^= (x >> 11);
		x ^= (x << 7) & 0x9D2C5680UL;
		x ^= (x << 15) & 0xEFC60000UL;
		return x ^ (x >> 18);
	}
private:



	static const int n = 624, m = 397; // compile time constants

	// the variables below are static (no duplicates can exist)
	uint32 m_nState[n+1]; // m_nState vector array
	int p; // position in m_nState array
	// private functions used to generate the pseudo random numbers
	uint32 twiddle(uint32 u, uint32 v) 
	{
		return (((u & 0x80000000UL) | (v & 0x7FFFFFFFUL)) >> 1)
			^ ((v & 1UL) ? 0x9908B0DFUL : 0x0UL);
	}
	void gen_state(); // generate new m_nState
	// make copy constructor and assignment operator unavailable, they don't make sense
	CMTRand_int32(const CMTRand_int32&); // copy constructor not defined
	void operator=(const CMTRand_int32&); // assignment operator not defined
};




#endif
