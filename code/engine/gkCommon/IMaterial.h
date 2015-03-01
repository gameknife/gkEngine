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
// Name:   	IMaterial.h
// Desc:	材质的抽象对象
// 	
// 
// Author:  YiKaiming
// Date:	2011/7/31
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _IMaterial_h_
#define _IMaterial_h_

#include "gkPlatform.h"
#include "IResource.h"
#include "IShader.h"

class gkMaterialPtr;
class gkTexturePtr;
class gkShaderPtr;
class IShader;

//! 默认材质路径，资源必须存在！
#define IMAT_DEFALUTNAME _T("engine/assets/meshs/_default.mtl")

	/**
	 @brief 材质参数类型枚举
	 @remark 
	*/
typedef enum _GKSHADERPARAMTYPE
{
	GKSHADERT_STRING = 0x1,				//! 字符串类型
	GKSHADERT_FLOAT = 0x2,				//! 浮点类型
	GKSHADERT_FLOAT2 = 0x3,				//! 二维向量
	GKSHADERT_FLOAT3 = 0x4,				//! 三维向量
	GKSHADERT_FLOAT4 = 0x5,				//! 四维向量
	GKSHADERT_DWORD  = 0x6,				//! 整形

	GKSHADERT_FORCEDWORD = 0x7fffffff
} GKSHADERPARAMTYPE;

	/**
	 @brief 材质参数结构
	 @remark 存储单一参数的结构，标识了参数名，参数类型，参数值和参数长度，以及最小最大值（供编辑器使用）
	*/
typedef struct _GKSHADERPARAM
{
	char                  pParamName[64];
	GKSHADERPARAMTYPE	  Type;           // type of the data pointed to by pValue
	DWORD                 NumBytes;       // size in bytes of the data pointed to by pValue
	void*				  pValue;         // data for the default of the effect

	// editor
	float				  minValue;
	float				  maxValue;

	_GKSHADERPARAM(const char* name, float value):minValue(0), maxValue(100)
	{
		strcpy(pParamName, name);

		Type = GKSHADERT_FLOAT;
		NumBytes = sizeof(float);
		pValue = new char[NumBytes];
		memcpy( pValue, &value, NumBytes );
	}
	_GKSHADERPARAM(const char* name, const Vec2& value):minValue(0), maxValue(100)
	{
		strcpy(pParamName, name);

		Type = GKSHADERT_FLOAT2;
		NumBytes = sizeof(Vec2);
		pValue = new char[NumBytes];
		memcpy( pValue, &value, NumBytes );
	}
	_GKSHADERPARAM(const char* name, const Vec3& value):minValue(0), maxValue(100)
	{
		strcpy(pParamName, name);

		Type = GKSHADERT_FLOAT3;
		NumBytes = sizeof(Vec3);
		pValue = new char[NumBytes];
		memcpy( pValue, &value, NumBytes );
	}
	_GKSHADERPARAM(const char* name, const Vec4& value):minValue(0), maxValue(100)
	{
		strcpy(pParamName, name);

		Type = GKSHADERT_FLOAT4;
		NumBytes = sizeof(Vec4);
		pValue = new char[NumBytes];
		memcpy( pValue, &value, NumBytes );
	}
	_GKSHADERPARAM(const char* name, const TCHAR* value):minValue(0), maxValue(100)
	{
		strcpy(pParamName, name);

		Type = GKSHADERT_STRING;
		NumBytes = (_tcslen(value) + 1) * sizeof(TCHAR);
		pValue = new char[NumBytes];
		memcpy( pValue, value, NumBytes );
	}
	_GKSHADERPARAM(const char* name, int value):minValue(0), maxValue(100)
	{
		strcpy(pParamName, name);

		Type = GKSHADERT_DWORD;
		NumBytes = sizeof(int);
		pValue = new char[NumBytes];
		memcpy( pValue, &value, NumBytes );
	}
	_GKSHADERPARAM(const _GKSHADERPARAM& other):minValue(0), maxValue(100)
	{
		strcpy(pParamName, other.pParamName);
		Type = other.Type;
		NumBytes = other.NumBytes;
		pValue = new char[NumBytes];
		memcpy(pValue, other.pValue, NumBytes);

		minValue = other.minValue;
		maxValue = other.maxValue;
	}

	/**
	 @brief 设置float类型值
	 @return 
	 @param const float & value
	 @remark 
	*/
	void setValue( const float& value )
	{
		if ( Type == GKSHADERT_FLOAT )
		{
			*((float*)pValue) = value;
		}
	}
	/**
	 @brief 设置Vec2类型值
	 @return 
	 @param const Vec2 & value
	 @remark 
	*/
	void setValue( const Vec2& value )
	{
		if ( Type == GKSHADERT_FLOAT2 )
		{
			*((Vec2*)pValue) = value;
		}
	}
	void setValue( const Vec3& value )
	{
		if ( Type == GKSHADERT_FLOAT3 )
		{
			*((Vec3*)pValue) = value;
		}
	}
	void setValue( const Vec4& value )
	{
		if ( Type == GKSHADERT_FLOAT4 )
		{
			*((Vec4*)pValue) = value;
		}
	}
	void setValue( const int& value )
	{
		if ( Type == GKSHADERT_DWORD )
		{
			*((int*)pValue) = value;
		}
	}
	void setValue( const TCHAR* value )
	{
		if ( Type == GKSHADERT_STRING )
		{
			delete[] (char*)pValue;
			NumBytes = (_tcslen(value) + 1) * sizeof(TCHAR);
			pValue = new char[NumBytes];
			memcpy( pValue, value, NumBytes );
		}
	}

	~_GKSHADERPARAM()
	{
		delete[] pValue;
	}

} GKSHADERPARAM;

	/**
	 @brief 材质参数表
	 @remark 参数表，承载材质的所有参数，提供参数清理，重新整理等操作。
			 提供对于参数表加载，重建，清理的接口。
	*/
struct gkMaterialParams
{
	typedef std::vector<GKSHADERPARAM*> ParameterList;

	gkMaterialParams() {
		m_vecParams.clear();
	}
	gkMaterialParams(const gkMaterialParams& other)
	{
		gkMaterialParams::ParameterList::const_iterator it = other.m_vecParams.begin();
		for (  ; it != other.m_vecParams.end(); ++it )
		{
			GKSHADERPARAM* cloned = new GKSHADERPARAM( **it );
			m_vecParams.push_back(cloned);
		}
	}

	virtual ~gkMaterialParams() {
		cleanParams();
	}
	
	void cleanParams()
	{
		gkMaterialParams::ParameterList::iterator it = m_vecParams.begin();
		for (  ; it != m_vecParams.end(); ++it )
		{
			delete *it;
		}

		m_vecParams.clear();
	}

	/**
	 @brief 根据名字取得参数
	 @return 
	 @param const char * paramName
	 @remark 如果存在参数，返回参数指针。否则返回NULL。
	*/
	GKSHADERPARAM* getParam(const char* paramName)
	{
		GKSHADERPARAM* ret = NULL;

		gkMaterialParams::ParameterList::iterator it = m_vecParams.begin();
		for (  ; it != m_vecParams.end(); ++it )
		{
			if ( !stricmp((*it)->pParamName, paramName) )
			{
				ret = *it;
				break;
			}
		}

		return ret;
	}

	/**
	 @brief 从另一个参数表重建
	 @return 
	 @param gkMaterialParams & other
	 @remark 根据另一个参数表的内容，添加缺失，删除多余，保留现有的参数和值。添加的参数使用另一个参数表中的值。
	*/
	void ripFromOther( gkMaterialParams& other)
	{
		// 先添加
		gkMaterialParams::ParameterList::const_iterator it = other.m_vecParams.begin();
		for (  ; it != other.m_vecParams.end(); ++it )
		{
			GKSHADERPARAM* paramThis = getParam( (*it)->pParamName );
			if (!paramThis)
			{
				GKSHADERPARAM* cloned = new GKSHADERPARAM( **it );
				m_vecParams.push_back(cloned);
			}
		}

		// 再清理
		gkMaterialParams::ParameterList::iterator itr = m_vecParams.begin();
		for (  ; itr != m_vecParams.end(); )
		{
			GKSHADERPARAM* paramThis = other.getParam( (*itr)->pParamName );
			if (!paramThis)
			{
				itr = m_vecParams.erase( itr );
			}
			else
			{
				++itr;
			}
		}
	}

	/// 材质名
	gkStdString m_wstrMatName;
	/// 参数表
	ParameterList m_vecParams;

	

};

	/**
	 @brief 加载类型枚举
	*/
enum GKMATERIAL_LOADTYPE
{
	GKMATERIAL_LOADTYPE_FILE,					///! 从文件加载
	GKMATERIAL_LOADTYPE_SCENE,					///! 从场景文件加载
	GKMATERIAL_LOADTYPE_INDIVIDUALFILE,			///! 独立文件加载
};

	/**
	 @brief 材质纹理通道枚举
	*/
enum EMaterialSlot
{
	eMS_Diffuse = 0,							///! 漫反射通道
	eMS_Normal,									///! 法线通道
	eMS_Specular,								///! 高光通道
	eMS_Detail,									///! 细节通道
	eMS_Custom1,								///! 预留自定义通道1
	eMS_Custom2,								///! 预留自定义通道2
	eMS_EnvMap,									///! 屏幕空间反射通道
	eMS_CubeMap,								///! cube环境反射通道

	eMS_Invalid,
};

	/**
	 @ingroup ResourceAPI
	 @brief 抽象材质对象
	*/
class UNIQUE_IFACE IMaterial : public IResource {
public:
	IMaterial(IResourceManager* creator, const gkStdString& name, gkResourceHandle handle,
		const gkStdString& group = _T("none")):IResource(eRT_Material, creator, name, handle, group) {}
	virtual ~IMaterial(void) {}

	// basic resource access


	/**
	 @brief 获取材质所用shader
	 @return 
	 @remark 
	*/
	virtual gkShaderPtr& getShader() =0;
	/**
	 @brief 获取纹理通道上的纹理对象
	 @return 
	 @param EMaterialSlot index
	 @remark EMaterialSlot的枚举值都可以取得，任何材质都会创建7个纹理。返回值理论上不会为空\n在gkENGINE中，内置材质类型将0-7号纹理通道固定为
	 diffuse, normal, specular, detail, custom0, custom1, env, cube这8张纹理。其中custom0和custom1在各个特殊着色方式中有不同用途
	*/
	virtual gkTexturePtr& getTexture(EMaterialSlot index) =0;
	/**
	 @brief 设置纹理通道上的纹理对象
	 @return 
	 @param gkTexturePtr & pTex
	 @param BYTE index
	 @remark 
	*/
	virtual void setTexture(gkTexturePtr& pTex, BYTE index = 0) =0;

	// basic shader technique access

	/**
	 @brief 提交材质的所有属性
	 @return 
	 @param bool texture 是否提交纹理
	 @param IShader * shader 提交到指定shader
	 @remark 一般使用，不带参数，参数均使用默认。则提交所有属性和参数，及逐个提交纹理。
			 \n设置texture = false, 则不提交纹理
			 \n给非空shader, 则将该材质参数提交到指定shader, 取代材质本身的shader。
	*/
	virtual void ApplyParameterBlock(bool texture = true, IShader* shader = NULL) =0;
	/**
	 @brief 提交基础属性
	 @return 
	 @param IShader * pShader
	 @remark 
	*/
	virtual void BasicApply(IShader* pShader) =0;

	// basic param access
	/**
	 @brief 取得UV重复次数
	 @return 
	 @remark 
	*/
	virtual Vec2& getUVTill() =0;
	/**
	 @brief 是否为双面材质
	 @return 
	 @remark 
	*/
	virtual bool isDoubleSide() =0;
	/**
	 @brief 取得光泽度
	 @return 
	 @remark 
	*/
	virtual float getGlossness() =0;
	/**
	 @brief 取得透明度
	 @return 
	 @remark 
	*/
	virtual int getOpacity() =0;
	/**
	 @brief 设置uv重复
	 @return 
	 @param Vec2 till
	 @remark 
	*/
	virtual void setUVTill(Vec2 till) =0;
	/**
	 @brief 设置UV偏移
	 @return 
	 @param float speed 偏移速度
	 @remark 
	*/
	virtual void setUVMovement(float speed) =0;
	/**
	 @brief 是否使用屏幕空间反射
	 @return 
	 @remark 
	*/
	virtual bool& getSSRL() =0;
	/**
	 @brief 是否产生阴影
	 @return 
	 @remark 
	*/
	virtual bool& getCastShadow() =0;

	/**
	 @brief 取得材质的参数表
	 @return 
	 @remark 
	*/
	virtual gkMaterialParams* getLoadingParameterBlock() =0;


	// material saving
	/**
	 @brief 存储材质到指定文件
	 @return 
	 @param const TCHAR * filename 指定文件名
	 @remark 
	*/
	virtual void saveAsMtlFile(const TCHAR* filename) =0;
	/**
	 @brief 存储材质到读取的文件
	 @return 
	 @remark 
	*/
	virtual void saveToMtlFile() =0;

	// submtl access

	/**
	 @brief 取得子材质个数
	 @return 
	 @remark 
	*/
	virtual uint8 getSubMtlCount() =0;
	/**
	 @brief 取得子材质指针
	 @return 
	 @param uint8 index
	 @remark 如果存在，返回指针（不带引用计数，非安全），否则返回NULL
	*/
	virtual IMaterial* getSubMaterial(uint8 index) =0;

	// MARCO相关

	/**
	 @brief 重建MARCO
	 @return 
	 @remark 
	*/
	virtual void rebuildMarco() {}
	/**
	 @brief 重建MARCO
	 @return 
	 @remark 
	*/
	virtual void changeShader(const TCHAR* shadername) {}

	virtual const gkStdString& getShaderName() =0;
	/**
	 @brief 取得shader marco
	 @return 
	 @remark 从材质加载的shader获取shader marco
			 \n一般用于编辑器获取marco list
	*/
	virtual ShaderMacros& getShaderMarcos() =0;
	/**
	 @brief 获取当前材质的shadermarco
	 @return 
	 @remark 
	*/
	virtual uint32 getShaderMarcoMask() =0;
};

	/**
	 @brief 抽象材质对象的共享指针
	*/
class UNIQUE_IFACE gkMaterialPtr : public SharedPtr<IMaterial> 
{
public:
	gkMaterialPtr() : SharedPtr<IMaterial>() {}
	explicit gkMaterialPtr(IMaterial* rep) : SharedPtr<IMaterial>(rep) {}
	gkMaterialPtr(const gkMaterialPtr& r) : SharedPtr<IMaterial>(r) {} 
	gkMaterialPtr(const gkResourcePtr& r) : SharedPtr<IMaterial>()
	{
		{
			pRep = static_cast<IMaterial*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}
	}

	/// Operator used to convert a ResourcePtr to a TexturePtr
	gkMaterialPtr& operator=(const gkResourcePtr& r)
	{
		if (pRep == static_cast<IMaterial*>(r.getPointer()))
			return *this;
		release();
		{
			pRep = static_cast<IMaterial*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}
		return *this;
	}
};

#endif // IMaterial_h_482a90f6_054f_4d41_89fc_bf714dd27c71
