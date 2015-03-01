#include "StableHeader.h"
#include "gkSkinRenderable.h"
#include "IResourceManager.h"
#include "IGameObjectLayer.h"
#include "CharacterInstance.h"
#include "Animation/Animation/Rig/hkaPose.h"
#include "gkRenderOperation.h"


//////////////////////////////////////////////////////////////////////////
gkSkinRenderable::gkSkinRenderable( void )
{
	m_uSubsetID = 0;
}
//////////////////////////////////////////////////////////////////////////
gkSkinRenderable::gkSkinRenderable( IGameObjectRenderLayer* pCreator, gkCharacterInstance* pCharacter, gkMeshPtr& pMesh, uint32 subsetID ):
	m_pCreator(pCreator)
	,m_pCharacter(pCharacter)
	,m_pMesh(pMesh)
	,m_uSubsetID(subsetID)
	,m_castShadow(true)
{
	m_pMaterial.setNull();
}
//////////////////////////////////////////////////////////////////////////
gkSkinRenderable::~gkSkinRenderable()
{
	m_pMaterial.setNull();
}
//////////////////////////////////////////////////////////////////////////
void gkSkinRenderable::setMaterial( const gkMaterialPtr& material )
{
	m_pMaterial = material;
}
//////////////////////////////////////////////////////////////////////////
void gkSkinRenderable::setMaterialName( const gkStdString& matName )
{
	m_wstrMatName = matName;
	gkMaterialPtr material = gEnv->pSystem->getMaterialMngPtr()->load(m_wstrMatName);
	if (material.isNull())
	{
		gkLogWarning(_T("gkSkinRenderable: [ %s:%d ] Loading Material Failed."), m_pCharacter->getParentGameObject()->getName().c_str(), m_uSubsetID );
		material = gEnv->pSystem->getMaterialMngPtr()->load(IMAT_DEFALUTNAME);
	}

	setMaterial(material);
}
//////////////////////////////////////////////////////////////////////////
AABB& gkSkinRenderable::getAABB()
{
	return getMesh()->GetAABB();
}
//////////////////////////////////////////////////////////////////////////
void gkSkinRenderable::getWorldTransforms( Matrix44* xform ) const
{
	*xform = m_CachedTransforms;
}
//////////////////////////////////////////////////////////////////////////
IMaterial* gkSkinRenderable::getMaterial( void )
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
//////////////////////////////////////////////////////////////////////////
void gkSkinRenderable::getRenderOperation( gkRenderOperation& op )
{
	m_pMesh->getRenderOperation(op, m_uSubsetID);
}
//////////////////////////////////////////////////////////////////////////
bool gkSkinRenderable::getSkinnedMatrix( Matrix44A** pMatrixs, uint32& size )
{
	if (!pMatrixs)
	{
		return true;
	}
			
	*pMatrixs = &(m_vecCachedTransforms[0]);
	size = m_vecCachedTransforms.size();
	
	return true;	
}
//////////////////////////////////////////////////////////////////////////
float gkSkinRenderable::getSquaredViewDepth( const ICamera* cam ) const
{
	return 0;
}
//////////////////////////////////////////////////////////////////////////
float gkSkinRenderable::getMaskColor() const
{
	return .1f;
}
//////////////////////////////////////////////////////////////////////////
void gkSkinRenderable::RT_Prepare()
{
	// decode from character instance
	// Grab the pose in model space
	const hkArray<hkQsTransform>& poseModelSpace = m_pCharacter->getPose()->getSyncedPoseModelSpace();

	IMesh::BoneBaseTransforms& bonebase = getMesh()->GetBoneBaseTransforms();
	int numBones = bonebase.size();

	// Compute the skinning matrix palette
	extArray<hkTransform> compositeWorldInverse( numBones );
	compositeWorldInverse.setSize( numBones );
	m_vecCachedTransforms.clear();
	// Multiply through by the bind pose inverse world inverse matrices, according to the skel to mesh bone mapping
	for (int p=0; p < numBones; p++)
	{
		Matrix44A* tmpMat = new Matrix44A;
		*tmpMat = bonebase[p];

		hkTransform transform;
		transform.set4x4ColumnMajor( &(tmpMat->m00) );
		compositeWorldInverse[p].setMul( poseModelSpace[ p ], transform );

		//poseModelSpace[ p ].copyToTransform( compositeWorldInverse[p] );

		Matrix44A* newMat = new Matrix44A;

		compositeWorldInverse[p].get4x4ColumnMajor( &(newMat->m00) );	

		m_vecCachedTransforms.push_back( *newMat );

		delete tmpMat;
		delete newMat;
	}


	m_CachedTransforms = m_pCreator->GetWorldMatrix();
}
