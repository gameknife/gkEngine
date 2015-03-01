#include "UIStableHeader.h"

#include "gkMovieMovableInstanceBase.h"
#include "gkMovieMovable.h"
#include "vtxColor.h"
#include "vtxDisplayObject.h"
#include "vtxDisplayObjectContainer.h"

//-----------------------------------------------------------------------------
// Desc: 顶点结构
//-----------------------------------------------------------------------------
struct VTXINSTANCEVERTEX
{
	float x,y,z;
	float nx,ny,nz;
	float dr,db,dg,da;
	float sr,sb,sg,sa;
	float tx,ty;

	const static D3DVERTEXELEMENT9 Decl[6];
};
//#define D3DFVF_VTXINSTANCEVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX0)

const D3DVERTEXELEMENT9 VTXINSTANCEVERTEX::Decl[6] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,  0 },
	{ 0, 24, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,  0 },
	{ 0, 40, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,  1 },
	{ 0, 56, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0 },
	D3DDECL_END()
};

#define Z_INTERVAL 30.0f

namespace vtx
{
	//-----------------------------------------------------------------------
	gkMovableInstanceBase::gkMovableInstanceBase(DisplayObject* display_object) 
		: mPacker(NULL), 
		mDisplayObject(display_object), 
		mVertex(NULL), 
		mParentMovable(NULL)
	{
		mRenderOp.vertexData = NULL;
		mRenderOp.indexData = NULL;

		mRenderOp.operationType = gkRenderOperation::OT_TRIANGLE_LIST; 
		mRenderOp.useIndexes = true;

		m_uVerticeBuffer = 800;
		m_uIndiceBuffer = 1200;

		// judge my real depth
		//uint uLayer = getLayer();
		float zDepth = 0;
		int nLayer = -3;
		DisplayObjectContainer* pFather = mDisplayObject->getParentContainer();
		while(pFather)
		{
			pFather = pFather->getParentContainer();
			nLayer++;
		}
		m_nCustomZOrder = nLayer;
	}
	//-----------------------------------------------------------------------
	gkMovableInstanceBase::~gkMovableInstanceBase()
	{

		SAFE_RELEASE( mRenderOp.vertexData );
		SAFE_RELEASE( mRenderOp.indexData );
		SAFE_RELEASE( mRenderOp.vertexDecl );

		mRenderOp.vertexData = NULL;
		mRenderOp.indexData = NULL;
	}
	//-----------------------------------------------------------------------
	const gkMaterialPtr& gkMovableInstanceBase::getMaterial() const
	{
		return mMaterial;
	}
	//-----------------------------------------------------------------------
	void gkMovableInstanceBase::getRenderOperation(gkRenderOperation& op)
	{
		op = mRenderOp;
	}
	//-----------------------------------------------------------------------
	void gkMovableInstanceBase::getWorldTransforms(D3DXMATRIX* xform) const
	{
		D3DXMATRIX gkmat = mParentMovable->_getParentNodeFullTransform();
		//xform[0] =  mWorldMatrix * gkmat;

		float zDepth = 0;
		int nLayer = -2;
		DisplayObjectContainer* pFather = mDisplayObject->getParentContainer();
		while(pFather)
		{
			pFather = pFather->getParentContainer();
			nLayer++;
		}
		m_nCustomZOrder = nLayer;

		D3DXMATRIX  matMul = mWorldMatrix;
		matMul._43 = -m_nCustomZOrder * Z_INTERVAL;
		xform[0] =  matMul * gkmat;
		//xform[0]._43 = zDepth;
	
		// DEBUG
		//Ogre::Matrix4 m = mWorldMatrix;
		//m[2][3] = (float)mDisplayObject->getZOrder();
		//xform[0] = ogre_mat * m;
	}

	//-----------------------------------------------------------------------
	void gkMovableInstanceBase::_lock()
	{
		HRESULT hr = mRenderOp.vertexData->Lock( 0, 0, (void**)&mVertex, NULL );
		assert(hr == S_OK);
	}
	//-----------------------------------------------------------------------
	void gkMovableInstanceBase::_unlock()
	{
		mRenderOp.vertexData->Unlock();
		mVertex = NULL;
	}
	//-----------------------------------------------------------------------
	void gkMovableInstanceBase::_createBuffers()
	{
		// do nothing
		mRenderOp.useIndexes = true;


		IDirect3DDevice9* m_pd3dDevice = gk3DEngine::getSingleton().getRenderSystemPtr()->getDevice();

		// TODO: 以后交有gkcore处理VB和IB，自动RESET和LOST，这里用了D3DPOOL_MANAGED来减少这种消耗
		// 以后需要用D3DPOOL_DEFAULT
		HRESULT hr = m_pd3dDevice->CreateVertexBuffer(
			m_uVerticeBuffer * sizeof(VTXINSTANCEVERTEX),
			D3DUSAGE_WRITEONLY,
			0,
			D3DPOOL_MANAGED,
			&(mRenderOp.vertexData),
			NULL
			);

		assert(hr == S_OK);

		hr = m_pd3dDevice->CreateIndexBuffer(
			m_uIndiceBuffer * sizeof(WORD),
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,		// 16位的index
			D3DPOOL_MANAGED,
			&(mRenderOp.indexData),
			NULL
			);

		assert(hr == S_OK);

		hr = m_pd3dDevice->CreateVertexDeclaration( VTXINSTANCEVERTEX::Decl, &(mRenderOp.vertexDecl) );

		assert(hr == S_OK);

		mRenderOp.vertexSize = sizeof(VTXINSTANCEVERTEX);
	}
	//-----------------------------------------------------------------------
	void gkMovableInstanceBase::_resizeBuffers(uint num_quads)
	{
 		if(!num_quads)
 		{
 			num_quads = 1;
 		}
 
 		mRenderOp.vertexCount = num_quads * 4;
 		mRenderOp.indexCount = num_quads * 6;

		mRenderOp.vertexStart = 0;
		mRenderOp.indexStart = 0;
		//SAFE_RELEASE( mRenderOp.vertexData );
		//SAFE_RELEASE( mRenderOp.indexData );

		


		WORD* index = NULL;
		HRESULT hr = mRenderOp.indexData->Lock( 0, 0, (void**)&index, NULL );

		assert(hr == S_OK);

 		for(uint i=0; i<num_quads; ++i)
 		{
			// d3d is ccw , differ from ogre
 			*index++ = 0+i*4;
 			*index++ = 2+i*4;
 			*index++ = 1+i*4;
 
 			*index++ = 0+i*4;
 			*index++ = 3+i*4;
 			*index++ = 2+i*4;
 		}
 
 		mRenderOp.indexData->Unlock();
	}
	//-----------------------------------------------------------------------
	void gkMovableInstanceBase::_addVertex(
		const D3DXVECTOR2& position, 
		const D3DXVECTOR2& texcoord, 
		const D3DXCOLOR& diffuse, 
		const D3DXCOLOR& specular)
	{
		// position
		*mVertex++ = position.x;
		*mVertex++ = position.y;
		*mVertex++ = 0;

		// normal
		*mVertex++ = 0;
		*mVertex++ = 0;
		*mVertex++ = -1;

		// diffuse
		*mVertex++ = diffuse.r;
		*mVertex++ = diffuse.g;
		*mVertex++ = diffuse.b;
		*mVertex++ = diffuse.a;

		// specular
		*mVertex++ = specular.r;
		*mVertex++ = specular.g;
		*mVertex++ = specular.b;
		*mVertex++ = specular.a;

		// texture coordinate
		*mVertex++ = texcoord.x;
		*mVertex++ = texcoord.y;
	}
	//-----------------------------------------------------------------------
	int gkMovableInstanceBase::getNumWorldTransform()
	{
		return 0;
	}
	//-----------------------------------------------------------------------
	float gkMovableInstanceBase::getSquaredViewDepth( const gkCamera* cam ) const
	{
		float squareDist = D3DXVec3LengthSq(&(cam->getDerivedPosition() - mParentMovable->getParentNode()->_getDerivedPosition()));
		return squareDist - (float)(m_nCustomZOrder * Z_INTERVAL);
	}
	//-----------------------------------------------------------------------
}