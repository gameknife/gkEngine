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
// Name:   	IShader.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2010/9/10
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _IShader_h_
#define _IShader_h_

#include "gkPlatform.h"
#include "IResource.h"
#include "ITexture.h"

struct ID3DXEffect;
struct D3DXMATRIX;
struct gkMaterialParams;

	/**
	 @brief shader的标准technique枚举
	 @remark 
	*/
enum EShaderInternalTechnique
{
	eSIT_General = 0,
	//eSIT_General_Skinned,
	eSIT_Zpass_DL,
	//eSIT_Zpass_Skined_DL,
	eSIT_ShadowPass,
	//eSIT_ShadowPass_Skined,

	eSIT_Zpass_DS,
	//eSIT_Zpass_Skined_DS,

	eSIT_ReflGenPass,

	eSIT_Count,
};
	/**
	 @brief shader宏结构
	 @remark 
	*/
struct ShaderMacro
{
	gkStdString m_name;
	gkStdString m_value;
	gkStdString m_desc;

	bool m_active;
	bool m_hidden;

	ShaderMacro()
	{
		m_active = false;
		m_hidden = false;
	}
};

enum eSystemShaderMacro
{
	eSSM_Skinned = 1<<31,
	eSSM_Forward = 1<<30,
	eSSM_LowProfile = 1<<29,
	eSSM_DefaultProfile = 1<<28,
	eSSM_HigheProfile = 1<<27,
	eSSM_Zpass = 1<<26,
};

typedef std::map<uint32, ShaderMacro> ShaderMacros;

	/**
	 @ingroup ResourceAPI
	 @brief 抽象shader对象
	 @remark 
	*/
class UNIQUE_IFACE IShader : public IResource {
public:
	IShader(IResourceManager* creator, const gkStdString& name, gkResourceHandle handle,
		const gkStdString& group = _T("none")):IResource(eRT_Shader, creator,name,handle,group) {}
	virtual ~IShader(void) {}

	virtual uint32 getDefaultRenderLayer() =0;

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

	// param设置

	/**
	 @brief 根据名字选择技术
	 @return 
	 @param LPCSTR name
	 @remark 
	*/
	virtual void FX_SetTechniqueByName(LPCSTR name) =0;
	/**
	 @brief 根据句柄选择技术
	 @return 
	 @param GKHANDLE hTech
	 @remark 
	*/
	virtual void FX_SetTechnique(GKHANDLE hTech) =0;
	/**
	 @brief 取得技术句柄
	 @return 
	 @param LPCSTR name
	 @remark 
	*/
	virtual GKHANDLE FX_GetTechniqueByName(LPCSTR name) =0;
	/**
	 @brief 取得标准tech的句柄
	 @return 
	 @param EShaderInternalTechnique tech
	 @remark 
	*/
	virtual GKHANDLE FX_GetTechnique(EShaderInternalTechnique tech) =0;
	/**
	 @brief 判断该技术是否为蒙皮技术
	 @return 
	 @param EShaderInternalTechnique tech
	 @remark 
	*/
	//virtual bool FX_TechniqueSkinned(EShaderInternalTechnique tech) =0;

	/**
	 @brief 设置静态寄存器的值
	 @return 
	 @param GKHANDLE hParam 寄存器句柄或名字
	 @param const void * data
	 @param uint32 bytes
	 @remark 
	*/
	virtual void FX_SetValue( GKHANDLE hParam, const void* data, uint32 bytes) =0;
	virtual void FX_SetFloat( GKHANDLE hParam, float data) =0;
	virtual void FX_SetFloat2( GKHANDLE hParam, const Vec2& data)  {}
	virtual void FX_SetFloat3( GKHANDLE hParam, const Vec3& data) =0;
	virtual void FX_SetFloat4( GKHANDLE hParam, const Vec4& data) =0;
	virtual void FX_SetMatrixArray(GKHANDLE hParam, D3DXMATRIX* data, uint32 size) =0;
	virtual void FX_SetMatrix(GKHANDLE hParam, const Matrix44& data) =0;


	/**
	 @brief 启动一个shader, 切换shader和所属状态
	 @return 
	 @param uint32 * pPasses
	 @param DWORD flag
	 @remark 
	*/
	virtual void FX_Begin(uint32* pPasses, DWORD flag) =0;
	/**
	 @brief 启动一个shader pass
	 @return 
	 @param uint32 uPass
	 @remark 
	*/
	virtual void FX_BeginPass(uint32 uPass) =0;
	/**
	 @brief 向设备提交寄存器常量
	 @return 
	 @remark 
	*/
	virtual void FX_Commit() =0;

	/**
	 @brief 关闭一个shader pass
	 @return 
	 @remark 
	*/
	virtual void FX_EndPass() =0;
	/**
	 @brief 关闭一个shader
	 @return 
	 @remark 
	*/
	virtual void FX_End() =0;

	/**
	 @brief 取得shader的默认参数表，供material调用
	 @return 参数表
	 @remark 
	*/
	virtual gkMaterialParams* getShaderDefaultParams() =0;
	/**
	 @brief 取得shader的预编译宏
	 @return 
	 @remark shader可支配的宏表，由gfx文件定义，这里读取出来供编辑器使用
	*/
	virtual ShaderMacros& getShaderMarcos() =0;
	/**
	 @brief 取得shader的shadermarco码
	 @return 
	 @remark 改码作为宏的组合标识，用于标识一份shader文件编译出的不同shader
	*/
	virtual uint32 getShaderMarcoMask() =0;

	/**
	 @brief 切换系统shader macro，前8位
	 @return 
	 @remark 该码将shader切换至系统状态，以实现单个shader的多种状态
	*/
	virtual void switchSystemMacro(uint32 systemMarcro = 0) {}

};

	/**
	 @brief shader抽象对象的共享指针
	 @return 
	 @remark 
	*/
class UNIQUE_IFACE gkShaderPtr : public SharedPtr<IShader> 
{
public:
	gkShaderPtr() : SharedPtr<IShader>() {}
	explicit gkShaderPtr(IShader* rep) : SharedPtr<IShader>(rep) {}
	gkShaderPtr(const gkShaderPtr& r) : SharedPtr<IShader>(r) {} 
	gkShaderPtr(const gkResourcePtr& r) : SharedPtr<IShader>()
	{
		{
			pRep = static_cast<IShader*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}
	}

	gkShaderPtr& operator=(const gkResourcePtr& r)
	{
		if (pRep == static_cast<IShader*>(r.getPointer()))
			return *this;
		release();
		{
			pRep = static_cast<IShader*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}
		return *this;
	}
};

#endif // IShader_h_78475478_d43a_4cd8_9350_e5472ab43ee3
