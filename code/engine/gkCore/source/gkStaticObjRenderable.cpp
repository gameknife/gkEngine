#include "gkStableHeader.h"
#include "gkStaticObjRenderable.h"
#include "gkEntity.h"
//#include "gkmaterialmanager.h"

#include "gkMemoryLeakDetecter.h"

//-----------------------------------------------------------------------
gkStaticObjRenderable::gkStaticObjRenderable( void ):
	m_pCreator(NULL)
	,m_wstrMatName(IMAT_DEFALUTNAME)
	,m_pMaterial(NULL)
	,m_pMesh(NULL)
	,m_uSubsetID(0)
	,m_castShadow(true)
	,m_matrixCaching(false)
{
	m_pMaterial.setNull();
}
//-----------------------------------------------------------------------
gkStaticObjRenderable::gkStaticObjRenderable( gkMeshPtr& pMesh, uint32 SubsetID ):
	m_pCreator(NULL)
	,m_wstrMatName(IMAT_DEFALUTNAME)
	,m_pMaterial(NULL)
	,m_pMesh(pMesh)
	,m_uSubsetID(SubsetID)
	,m_castShadow(true)
	,m_matrixCaching(false)
{
	m_pMaterial.setNull();
}
//-----------------------------------------------------------------------
gkStaticObjRenderable::~gkStaticObjRenderable()
{
}
//-----------------------------------------------------------------------
void gkStaticObjRenderable::setMaterialName( const gkStdString& matName )
{
	m_wstrMatName = matName;
	gkMaterialPtr material = gEnv->pSystem->getMaterialMngPtr()->load(m_wstrMatName);
	if (material.isNull())
	{
		gkLogWarning(_T("gkStaticObjRenderable: [ %s:%d ] Loading Material Failed."), m_pCreator->getParentGameObject()->getName().c_str(), m_uSubsetID );
		material = gEnv->pSystem->getMaterialMngPtr()->load(IMAT_DEFALUTNAME);
	}

	setMaterial(material);
}
//-----------------------------------------------------------------------
void gkStaticObjRenderable::setMaterial( const gkMaterialPtr& material )
{
	m_pMaterial = material;
}
//-----------------------------------------------------------------------
IMaterial* gkStaticObjRenderable::getMaterial( void )
{
	// if got material
	if ( !(m_pMaterial.isNull()))
	{
		if (m_pMaterial->getSubMtlCount() > 0)
		{
			return m_pMaterial->getSubMaterial(m_uSubsetID);
		}
		else
		{
			return m_pMaterial.getPointer();
		}
	}

	// if not find material, find parents
	if( m_pCreator->getMaterial() )
	{
		if (m_pCreator->getMaterial()->getSubMtlCount() > 0)
		{
			return m_pCreator->getMaterial()->getSubMaterial(m_uSubsetID);
		}
		else
		{
			return m_pCreator->getMaterial();
		}
	}

	// if not get parents, return default
	m_pCreator->setMaterialName(IMAT_DEFALUTNAME);
	//m_pMaterial = gEnv->pSystem->getMaterialMngPtr()->getByName(IMAT_DEFALUTNAME);
	return NULL;
}
//-----------------------------------------------------------------------
void gkStaticObjRenderable::getRenderOperation( gkRenderOperation& op )
{
	m_pMesh->getRenderOperation(op, m_uSubsetID);
}
//-----------------------------------------------------------------------
void gkStaticObjRenderable::getWorldTransforms( Matrix44* xform ) const
{
	if (m_matrixCaching)
	{
		*xform = m_CachedTransform;
	}
	else
	{
		*xform = m_pCreator->GetWorldMatrix();
	}

	m_pCreator->GetWorldMatrix();
	//xform->Transpose();
}
//-----------------------------------------------------------------------
float gkStaticObjRenderable::getSquaredViewDepth( const ICamera* cam ) const
{
	// TODO
	return 0;
}

float gkStaticObjRenderable::getMaskColor() const
{
	//if (m_pCreator)
	//	return m_pCreator->getMaskColor();
	return 0.f;
}
//////////////////////////////////////////////////////////////////////////
AABB& gkStaticObjRenderable::getAABB()
{
	return getMesh()->GetAABB();
}
//////////////////////////////////////////////////////////////////////////
gkMeshPtr& gkStaticObjRenderable::getMesh()
{
	return m_pMesh;
}
//////////////////////////////////////////////////////////////////////////
void gkStaticObjRenderable::RT_Prepare()
{
	m_CachedTransform = m_pCreator->GetWorldMatrix();
	m_matrixCaching = true;
}

