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
// Name:   	gkPhysicsTerrian.h
// Desc:	
// 	
// 
// Author:  Kaiming
// Date:	2013/1/17	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkPhysicsTerrian_h__
#define gkPhysicsTerrian_h__

#include "ITexture.h"
#include <Physics2012/Collide/Shape/HeightField/SampledHeightField/hkpSampledHeightFieldShape.h>
#include <Physics2012/Collide/Shape/HeightField/SampledHeightField/hkpSampledHeightFieldBaseCinfo.h>

class gkPhysicsTerrianProxy : public hkpSampledHeightFieldShape
{
public:

	gkPhysicsTerrianProxy( const hkpSampledHeightFieldBaseCinfo& ci, hkReal* data ): hkpSampledHeightFieldShape(ci) , m_data(data)
	{
	}

	/// Generate a rough terrain
	HK_FORCE_INLINE hkReal getHeightAtImpl( int x, int z ) const
	{
		return m_data[x * m_zRes + z];
	}

	///		this should return true if the two triangles share the edge p00-p11
	///     otherwise it should return false if the triangles sphere the edge p01-p10
	HK_FORCE_INLINE hkBool getTriangleFlipImpl() const
	{	
		return false;
	}

	virtual void collideSpheres( const CollideSpheresInput& input, SphereCollisionOutput* outputArray) const
	{
		hkSampledHeightFieldShape_collideSpheres(*this, input, outputArray);
	}


	hkReal* m_data;
};

class gkPhysicsTerrian
{
public:
	gkPhysicsTerrian( const gkTexturePtr& heightmap, float maxHeight, float zeroOffset );

	~gkPhysicsTerrian();

	gkPhysicsTerrianProxy* m_proxy;
};

#endif // gkPhysicsTerrian_h__

