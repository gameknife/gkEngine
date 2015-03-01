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
// Name:   	gkMaterial.h
// Desc:	todo...
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	9/10/2010 	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkMaterial_h_482a90f6_054f_4d41_89fc_bf714dd27c71
#define gkMaterial_h_482a90f6_054f_4d41_89fc_bf714dd27c71

#include "RendererD3D9Prerequisites.h"
#include "IMaterial.h"
#include "IShader.h"
#include "ITexture.h"
#include <vector>

struct CRapidXmlParseNode;
struct CRapidXmlAuthorNode;
class gkTexture;

class gkMaterial : public IMaterial {
public:
	gkMaterial(IResourceManager* creator, const gkStdString& name, gkResourceHandle handle,
		const gkStdString& group = _T("none"), gkNameValuePairList* params = NULL);
	virtual ~gkMaterial(void);

	/**
	 @brief getShader 取得材质使用的着色器
	 @return gkShaderPtr&
	 @remark 
	*/
	gkShaderPtr& getShader();
	/**
	 @brief getTexture 取得指定插槽上的材质纹理
	 @return gkTexturePtr&
	 @param EMaterialSlot index
	 @remark 
	*/
	gkTexturePtr& getTexture(EMaterialSlot index);
	void setTexture(gkTexturePtr& pTex, BYTE index = 0);

	bool isDoubleSide() {return m_bDoubleSide;}
	virtual float getGlossness();
	virtual int getOpacity();
	virtual bool& getSSRL() {return m_bSSRL;}

	virtual void saveAsMtlFile(const TCHAR* filename);
	virtual void saveToMtlFile();

	virtual void setUVTill(Vec2 till) {m_vUVTill = till;}
	virtual void setUVMovement(float speed) {m_fUVSpeed = speed;}

	virtual gkMaterialParams* getLoadingParameterBlock()
	{
		return m_pLoadingParamBlock;
	}

	virtual void ApplyParameterBlock( bool texture, IShader* shader );
	virtual void BasicApply(IShader* pShader);

	virtual Vec2& getUVTill() {return m_vUVTill;}
	virtual bool& getCastShadow() {return m_castShadow;}

	// submtl access
	virtual uint8 getSubMtlCount() {return m_uSubMtlCount;}
	virtual IMaterial* getSubMaterial(uint8 index);

	// 目前方式，通过定义的paramblock来生成
	virtual bool loadFromParamBlock();

	virtual bool saveMaterialToRapidXmlNode( CRapidXmlAuthorNode* node );

	virtual void rebuildMarco();
	virtual void changeShader(const TCHAR* shadername);
	virtual ShaderMacros& getShaderMarcos() {return m_shaderMacros;}
	virtual uint32 getShaderMarcoMask() {return m_marcoMask;}
	virtual const gkStdString& getShaderName() {return m_shaderFilename;}

protected:

	gkMaterial* getSubMtl( const TCHAR* name);

	virtual bool loadImpl(void);			// 继承实现
	virtual bool unloadImpl(void);

	virtual bool loadMaterialFormRapidXmlNode( CRapidXmlParseNode* node );

	void LoadTexturesFromXmlNode(CRapidXmlParseNode* fxXmlNode);

	void ApplyParams(IShader* shader);
	void releaseParameterBlock();

private:
	gkShaderPtr m_pShader;						// 实际的shader对象
	gkNameValuePairList loadingParams;

	typedef std::vector<gkTexturePtr> TextureArray; 
	TextureArray m_vecTextureArray;				// 该材质所包含的贴图对象

	bool		m_bTransparent;					// 是否为半透明材质
	bool		m_bDoubleSide;
	bool		m_bSSRL;
	float		m_fGlossness;
	float*		m_nOpacity;
	float		m_alphaRef;

	gkStdString		m_wstrIndividualFileName;
	BYTE				m_yLoadType;
	gkMaterialParams*	m_pLoadingParamBlock;

	bool        m_bLoadFromFile;
	bool		m_castShadow;

	Vec2		m_vUVTill;
	Vec2		m_vUVOffset;
	float		m_fUVSpeed;

	uint8		m_uSubMtlCount;
	typedef std::vector<gkMaterialPtr> SubMaterialList;
	SubMaterialList	m_vecSubMtlList;

	gkStdString m_shaderFilename;
	bool		m_shaderFileSet;

	ShaderMacros m_shaderMacros;
	uint32 m_marcoMask;
};

#endif // gkMaterial_h_482a90f6_054f_4d41_89fc_bf714dd27c71
