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
// yikaiming (C) 2013
// gkENGINE Source File 
//
// Name:   	ITexture.h
// Desc:	抽象纹理对象
// 	
// 
// Author:  YiKaiming
// Date:	2013/6/17
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _ITexture_h_
#define _ITexture_h_

#include "gkPlatform.h"
#include "IResource.h"

struct IDirect3DTexture9;

enum ETexutureFormat
{
	eTF_RGB4,
	eTF_RGB5,
	eTF_RGB8,
	eTF_RGB10,
	eTF_RGB12,
	eTF_RGB16,
	eTF_RGBA2,
	eTF_RGBA4,
	eTF_RGB5_A1,
	eTF_RGBA8,
	eTF_RGB10_A2,
	eTF_RGBA12,
	eTF_RGBA16,

	eTF_A8,
	eTF_R8,

	eTF_RGBA32F,
	eTF_RGB32F,
	eTF_RGBA16F,
	eTF_RGB16F,

	eTF_RG16F,

	eTF_R32F,
	eTF_R16F,

	eTF_DXT5,
	eTF_DXT3,
	eTF_DXT1,

	eTF_NULL,
	eTF_INTZ,

	eTF_UnKnown,
};

inline uint32 getFormatBPP( ETexutureFormat etf )
{
	uint32 bpp = 0;
	switch( etf )
	{
	case eTF_RGB8:
	case eTF_RGBA8:
	case eTF_R32F:
	case eTF_RG16F:
	case eTF_INTZ:
		bpp = 32;
		break;
	case eTF_DXT1:
	case eTF_A8:
	case eTF_R8:
		bpp = 4;
		break;
	case eTF_DXT3:
	case eTF_DXT5:
		bpp = 8;
		break;
	case eTF_RGBA32F:
	case eTF_RGB32F:
		bpp = 32 * 4;
		break;
	case eTF_RGBA16F:
	case eTF_RGB16F:
		bpp = 16 * 4;
		break;
	case eTF_R16F:
		bpp = 16;
		break;
	}

	return bpp;
}

inline ETexutureFormat getTextureFormat(const gkStdString& format)
{
	if(format == _T("A32B32G32R32F"))
	{
		return eTF_RGBA32F;
	}
	else if (format == _T("R8G8B8A8"))
	{
		return eTF_RGBA8;
	}
	else if (format == _T("A8R8G8B8"))
	{
		return eTF_RGBA8;
	}
	else if (format == _T("R8G8B8"))
	{
		return eTF_RGB8;
	}
	else if (format == _T("A16B16G16R16F"))
	{
		return eTF_RGBA16F;
	}
	else if (format == _T("G16R16F"))
	{
		return eTF_RG16F;
	}
	else if (format == _T("R32F"))
	{
		return eTF_R32F;
	}
	else if (format == _T("R16F"))
	{
		return eTF_R16F;
	}
	else if (format == _T("INTZ"))
	{
		return eTF_INTZ;
	}
	else if ( format == _T("NULL"))
	{
		return eTF_NULL;
	}
	else
	{
		return eTF_UnKnown;
	}
}

inline bool isDepthTexture( ETexutureFormat etf )
{
	switch( etf )
	{
	case eTF_INTZ:
		return true;
	default:
		return false;
	}
}

struct gkLockOperation
{
	enum ELockFlag
	{
		eLF_Discard,
		eLF_Keep,
		eLF_Read,
	};

	ELockFlag m_flag;
	int m_level;
	Vec2i m_pos;
	Vec2i m_size;

	void* m_outData;
	int m_outPitch;


	gkLockOperation(): m_flag(eLF_Discard)
		,m_level(0)
		,m_pos(0,0)
		,m_size(0,0)
		,m_outData(NULL)
		,m_outPitch(0)
	{
	}
};

	/**
	 @ingroup ResourceAPI
	 @brief 抽象纹理对象，提供纹理的外部访问接口
	 @remark 
	*/
struct UNIQUE_IFACE ITexture : public IResource {
public:
	ITexture(IResourceManager* creator, const gkStdString& name, gkResourceHandle handle,
		const gkStdString& group = _T("none")):IResource(eRT_Texture, creator,name,handle,group) {}
	virtual ~ITexture(void) {}

	/**
	 @brief 将纹理绑定于设备的纹理寄存器
	 @return 
	 @param uint32 channel 通道
	 @param uint8 filter 采样方式
	 @remark 
	*/
	virtual void Apply(uint32 channel, uint8 filter) =0;

	/**
	 @brief 获得纹理的高度
	 @return 
	 @remark 
	*/
	virtual uint32 getHeight() =0; 
	/**
	 @brief 获得纹理的宽度
	 @return 
	 @remark 
	*/
	virtual uint32 getWidth() =0; 
	/**
	@brief 获得纹理的深度
	@return
	@remark
	*/
	virtual uint32 getMipLevel() = 0;
	/**
	 @brief DX9设备重设接口
	 @return 
	 @remark 
	*/
	virtual void onReset() =0;
	/**
	 @brief DX9设备重设接口
	 @return 
	 @remark 
	*/
	virtual void onLost() =0;

	/**
	 @brief 对RAW纹理采样
	 @return 
	 @param const Vec2 & texcoord 采样纹理坐标
	 @param int filter 是否使用linear过滤
	 @remark 只对raw纹理作用
	*/
	virtual float Tex2DRAW(const Vec2& texcoord, int filter = 1) =0;
	/**
	 @brief 获得RAW纹理的数据指针
	 @return 
	 @remark 只对raw纹理作用
	*/
	virtual uint8* RawData() =0;
	/**
	 @brief 改变纹理属性，用于改变动态纹理的尺寸/格式等
	 @return 
	 @param const gkStdString & key 属性名
	 @param const gkStdString & value 属性值
	 @remark 使用字符串，牺牲性能换取灵活性，尽量不要每帧调用
	*/
	virtual void changeAttr(const gkStdString& key, const gkStdString& value) =0;
	/**
	 @brief 获取纹理属性
	 @return 
	 @param const gkStdString & key
	 @remark 
	*/
	virtual const gkStdString& getAttr(const gkStdString& key) const=0;
	/**
	 @brief 生成Mipmap, 只对具有Mipmap属性的动态纹理作用
	 @return 
	 @remark 
	*/
	virtual void AutoGenMipmap() {}


	virtual bool lock(gkLockOperation& lockopt) {return false;}

	virtual void unlock() {}

	virtual bool dynamic(){return false;}

	virtual bool sizable(){return false;}

	virtual ETexutureFormat format() =0;

	virtual void calcMemUsage() {}
};





	/**
	 @brief 纹理对象的共享指针
	 @remark 
	*/
class UNIQUE_IFACE gkTexturePtr : public SharedPtr<ITexture> 
{
public:
	gkTexturePtr() : SharedPtr<ITexture>() {}
    gkTexturePtr(ITexture* rep) : SharedPtr<ITexture>(rep) {}
	gkTexturePtr(const gkTexturePtr& r) : SharedPtr<ITexture>(r) {} 
	gkTexturePtr(const gkResourcePtr& r) : SharedPtr<ITexture>()
	{
		{
			pRep = static_cast<ITexture*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}
	}

	gkTexturePtr& operator=(const gkResourcePtr& r)
	{
		if (pRep == static_cast<ITexture*>(r.getPointer()))
			return *this;
		release();
		{
			pRep = static_cast<ITexture*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}
		return *this;
	}
};
typedef std::vector<gkTexturePtr> gkTexturePtrs;

#endif // ITexture_h_2c9c5d72_40e8_4f7a_b302_edb71138545a
