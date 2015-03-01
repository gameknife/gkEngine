#include "UIStableHeader.h"
#include "gkVtxTexture.h"
#include "vtxRect.h"
#include "vtxStringHelper.h"
#include <winbase.h>

namespace vtx{
	//-----------------------------------------------------------------------
	gkVtxTexture::gkVtxTexture(const uint& width, const uint& height) 
		: Texture(width, height)
	{
		// 创建用于渲染的贴图
		gkStdStringstream wss;
		wss<<((int)this);
		m_pTexture = gEnv->pSystem->getTextureMngPtr()->create(wss.str(), _T(""));


		LPDIRECT3DTEXTURE9 pTexture = NULL;
		HRESULT hr = gEnv->pRenderer->getDevice()->CreateTexture(width,
			height,
			1,
			D3DUSAGE_AUTOGENMIPMAP,
			D3DFMT_A8R8G8B8,
			D3DPOOL_MANAGED,
			&pTexture,
			NULL );


		assert(hr == S_OK);

		//m_pTexture->_setTexture(pTexture);

		// 创建用于渲染的材质
		m_pMaterial = gkMaterialManager::getSingleton().create(wss.str(),_T(""));
		//m_pMaterial->setLoadingIndividualFile(_T("ksMat_DefaultVtx.X"));
		// 这里其实会产生一个多于的默认贴图，不过已经把他压缩到较小了 [4/7/2011 Kaiming-Desktop]
		m_pMaterial->load();
		m_pMaterial->setTexture(m_pTexture);
	}
	//-----------------------------------------------------------------------
	gkVtxTexture::~gkVtxTexture()
	{
		// 销毁纹理
		m_pTexture->unload();
		m_pTexture.setNull();
	}

	//-----------------------------------------------------------------------
	void gkVtxTexture::paintPixelsToRect(const Rect& coordinates, unsigned char* pixelData)
	{
		// 取得texture的surface并填充
		RECT winRectDest;
		winRectDest.left = static_cast<LONG>(coordinates.left);
		winRectDest.right = static_cast<LONG>(coordinates.right);
		winRectDest.top = static_cast<LONG>(coordinates.top);
		winRectDest.bottom = static_cast<LONG>(coordinates.bottom);

		RECT winRectSrc;
		winRectSrc.left = static_cast<LONG>(0);
		winRectSrc.right = static_cast<LONG>(coordinates.w());
		winRectSrc.top = static_cast<LONG>(0);
		winRectSrc.bottom = static_cast<LONG>(coordinates.h());

		UINT mul = GetPrivateProfileIntA("BulkTest", "pitchmulti", 4,"D:\\test\\test.ini");
 		UINT count = GetPrivateProfileIntA("BulkTest", "pitchstart", 4,"D:\\test\\test.ini");
// 		D3DFORMAT FMT = (D3DFORMAT)GetPrivateProfileIntA("BulkTest", "format", D3DFMT_A8R8G8B8,"D:\\test\\test.ini");
// 
// 		UINT pitch = mul;
// 		for(UINT i = 0; i < count; i++)
// 			pixelData++;

		// 高难动作！
		IDirect3DSurface9* pTexSurf;

		RECT dirty;
		dirty.left = 0;
		dirty.right = 2048;
		dirty.top = 0;
		dirty.bottom = 2048;
		m_pTexture->getTexture()->AddDirtyRect(&winRectDest);

		HRESULT hr = m_pTexture->getTexture()->GetSurfaceLevel(0, &pTexSurf);
		hr = D3DXLoadSurfaceFromMemory(pTexSurf, NULL, &winRectDest, 
			(void*)pixelData, D3DFMT_A8R8G8B8,
			winRectSrc.right * mul,
			NULL, &winRectSrc, D3DX_DEFAULT, 0);

// 		hr =D3DXSaveSurfaceToFile(
// 			_T("testimg.jpg"),
// 			D3DXIFF_JPG,
// 			pTexSurf,
// 			NULL,
// 			&winRect
// 			);

		assert(hr == S_OK);

		SAFE_RELEASE( pTexSurf );
	}
	//-----------------------------------------------------------------------
	const gkMaterialPtr& gkVtxTexture::getMaterial() const
	{
		return m_pMaterial;
	}

}