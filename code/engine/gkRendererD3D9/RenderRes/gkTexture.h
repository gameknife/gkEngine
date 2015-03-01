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

#ifndef gkTexture_h_2c9c5d72_40e8_4f7a_b302_edb71138545a
#define gkTexture_h_2c9c5d72_40e8_4f7a_b302_edb71138545a

#include "RendererD3D9Prerequisites.h"

#include "ITexture.h"

struct ITask;

inline D3DFORMAT getD3DFormatFromETF( ETexutureFormat etf )
{
	switch( etf )
	{
	case eTF_RGB8:
		return D3DFMT_R8G8B8;
	case eTF_RGBA8:
		return D3DFMT_A8R8G8B8;
	case eTF_RGBA16F:
		return D3DFMT_A16B16G16R16F;
	case eTF_RGBA32F:
		return D3DFMT_A32B32G32R32F;
	case eTF_RG16F:
		return D3DFMT_G16R16F;
	case eTF_R16F:
		return D3DFMT_R16F;
	case eTF_R32F:
		return D3DFMT_R32F;
	case eTF_A8:
		return D3DFMT_A8;
	case eTF_DXT1:
		return D3DFMT_DXT1;
	case eTF_DXT3:
		return D3DFMT_DXT3;
	case eTF_DXT5:
		return D3DFMT_DXT5;
	case eTF_INTZ:
		return (D3DFORMAT)MAKEFOURCC('I','N','T','Z');
	case eTF_NULL:
		return D3DFMT_A8R8G8B8;
	default:
		return D3DFMT_UNKNOWN;
	}
}

class gkTexture : public ITexture,
	public IFileChangeMonitorListener

{
protected:
	D3DPOOL m_dwD3DPool;					// D3DPOOL
	IDirect3DTexture9*	m_p2DTexture;
	IDirect3DCubeTexture9*  m_pCubeTexture;
	bool	m_bIsManaged;					// 是否由别人托管

	bool	m_bIsDefaultPool;
	gkNameValuePairList loadingParams;

	uint32 m_uWidth;
	uint32 m_uHeight;

	gkStdString m_wstrFormat;

	uint8* m_rawData;
	int m_shrinkLevel;
	bool m_rawDirty;

	ETexutureFormat m_format;

public:
	//gkTexture(void);
	gkTexture(IResourceManager* creator, const gkStdString& name, gkResourceHandle handle,
		const gkStdString& group = _T("none"), gkNameValuePairList* params = NULL);
	virtual ~gkTexture(void);

	IDirect3DBaseTexture9* getTexture();
	IDirect3DTexture9* get2DTexture();

	virtual void Apply(uint32 channel, uint8 filter);

	virtual uint32 getHeight() {return m_uHeight;} 
	virtual uint32 getWidth() {return m_uWidth;}  

	virtual void onReset();
	virtual void onLost();

	virtual float Tex2DRAW(const Vec2& texcoord, int filter = 1);
	float Tex2DRAW(const Vec2i& texcoord);
	virtual uint8* RawData() {return m_rawData;}
	virtual void RawDataFlush();
	virtual void AutoGenMipmap();

	virtual bool lock(gkLockOperation& lockopt);
	virtual void unlock();
	
	virtual ETexutureFormat format() {return m_format;}

protected:
	virtual bool loadImpl(void);			// 继承实现
	virtual bool loadImpl(IDirect3DDevice9* d3d9Device);		//dx的构建函数
	virtual bool unloadImpl(void);

	virtual void changeAttr( const gkStdString& key, const gkStdString& value );

	virtual const gkStdString& getAttr(const gkStdString& key) const;

	virtual void OnFileChange( const TCHAR* filename );

	virtual bool dynamic(){return m_bIsDefaultPool;}

	virtual bool sizable(){return m_bSizeable;}

	virtual void calcMemUsage();

	ITask* m_loadingTask;
	int m_lastLockLevel;
	bool m_bSizeable;
};


#endif // gkTexture_h_2c9c5d72_40e8_4f7a_b302_edb71138545a
