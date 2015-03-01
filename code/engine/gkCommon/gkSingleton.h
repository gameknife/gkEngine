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



//////////////////////////////////////////////////////////////////////////
//
// Name:   	gkSingleton.h
// Desc:	todo...
// 描述:	
// 
// Author:  Kaiming-Laptop
// Date:	2010/9/13
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _SINGLETON_H__
#define _SINGLETON_H__

#include <assert.h>

template <typename T> class Singleton
{
private:
	/** \brief Explicit private copy constructor. This is a forbidden operation.*/
	Singleton(const Singleton<T> &);

	/** \brief Private operator= . This is a forbidden operation. */
	Singleton& operator=(const Singleton<T> &);

protected:

	static T* ms_Singleton;

public:
	Singleton( void )
	{
		assert( !ms_Singleton );
		ms_Singleton = static_cast< T* >( this );
	}
	~Singleton( void )
	{  assert( ms_Singleton );  ms_Singleton = 0;  }
	static T& getSingleton( void )
	{	assert( ms_Singleton );  return ( *ms_Singleton ); }
	static T* getSingletonPtr( void )
	{ return ms_Singleton; }
};

#endif
