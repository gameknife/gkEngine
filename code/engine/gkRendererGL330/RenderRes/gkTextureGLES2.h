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
// Name:   	gkTexture.h
// Desc:	todo...
// 描述:	
// 
// Author:  Kaiming-Laptop
// Date:	2010/9/10
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkTextureGLES2_h_
#define gkTextureGLES2_h_

#include "Prerequisites.h"

#include "ITexture.h"


class gkTextureGLES2 : public ITexture {
    
public:
    friend class gkTexture2DGLES2StreamingTask;
    
protected:
	bool	m_bIsManaged;					// 是否由别人托管

	bool	m_bIsDefaultPool;
	gkNameValuePairList loadingParams;

	uint32 m_uWidth;
	uint32 m_uHeight;

	gkStdString m_wstrFormat;

	GLuint m_uHwTexture2D;
	GLuint m_uHwTextureCube;

	bool m_rt;

	bool m_bCubemap;

	uint8* m_rawData;

	bool m_rawDirty;

	ETexutureFormat m_format;

	bool m_dynamic;

public:
	gkTextureGLES2(IResourceManager* creator, const gkStdString& name, gkResourceHandle handle,
		const gkStdString& group = _T("none"), gkNameValuePairList* params = NULL);
	virtual ~gkTextureGLES2(void);

	virtual void Apply(uint32 channel, uint8 filter);

	virtual uint32 getHeight() {return m_uHeight;} 
	virtual uint32 getWidth() {return m_uWidth;}  

	virtual void onReset();
	virtual void onLost();

	virtual bool lock( gkLockOperation& lockopt );
	virtual void unlock();

	virtual ETexutureFormat format() {return m_format;}

	void setCube(bool isCube) { m_bCubemap = isCube; }

	GLuint getGLHandle() {return m_uHwTexture2D;}

	virtual bool dynamic(){ return m_dynamic; }
protected:
	virtual bool loadImpl(void);			// 继承实现
	virtual bool unloadImpl(void);

	virtual float Tex2DRAW( const Vec2& texcoord, int filter = 1 );
	virtual float Tex2DRAW( const Vec2i& texcoord );

	virtual uint8* RawData() {return m_rawData;}

	void RawDataFlush();

	virtual void changeAttr( const gkStdString& key, const gkStdString& value );

	virtual const gkStdString& getAttr( const gkStdString& key ) const;
	
};


#endif // gkTexture_h_2c9c5d72_40e8_4f7a_b302_edb71138545a
