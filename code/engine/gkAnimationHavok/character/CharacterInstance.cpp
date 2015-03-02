#include "StableHeader.h"
#include "CharacterInstance.h"
#include "IXmlUtil.h"
#include "Animation/Animation/Playback/hkaAnimatedSkeleton.h"
#include "Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h"
#include "Animation/Animation/Rig/hkaPose.h"
#include "IRenderer.h"
#include "IAuxRenderer.h"
#include "I3DEngine.h"
#include "IResourceManager.h"
#include "gkSkinRenderable.h"
#include "../utils/gkHavokAssetManagementUtil.h"


//////////////////////////////////////////////////////////////////////////
gkCharacterInstance::gkCharacterInstance( const TCHAR* name )
{
	m_wstrName = name;
	m_pSkeleton = NULL;
	m_mapAnimations.clear();
	m_pPose = NULL;
}
//////////////////////////////////////////////////////////////////////////
bool gkCharacterInstance::loadChrFile( const TCHAR* filename )
{
	IRapidXmlParser parser;
	parser.initializeReading(filename);

	CRapidXmlParseNode* pChrRoot = parser.getRootXmlNode(_T("Character"));
	CRapidXmlParseNode* pChrRig = pChrRoot->getChildNode(_T("Rig"));
	CRapidXmlParseNode* pChrMesh = pChrRoot->getChildNode(_T("Mesh"));
	CRapidXmlParseNode* pChrAnimationList = pChrRoot->getChildNode(_T("AnimationList"));

	// building rig
	hkaSkeleton* pSkeleton = getAnimationPtr()->getRigManager().loadRig( pChrRig->GetAttribute(_T("FileName")) );
	if (pSkeleton)
	{
		m_pSkeleton = new hkaAnimatedSkeleton( pSkeleton );

		m_pPose = new hkaPose(m_pSkeleton->getSkeleton());

		// building animation list

		hkaAnimationBinding* bindings = NULL;

		CRapidXmlParseNode* pChrAnimations = pChrAnimationList->getChildNode(_T("Animation"));
		for( ; pChrAnimations; pChrAnimations = pChrAnimations->getNextSiblingNode() )
		{
			bindings = getAnimationPtr()->getAnimationManager().loadAnimation(pChrAnimations->GetAttribute(_T("filename")));
			if (bindings)
			{
				hkaDefaultAnimationControl* animationControl = new hkaDefaultAnimationControl( bindings );
				m_mapAnimations.insert(AnimationControlMap::value_type(pChrAnimations->GetAttribute(_T("name")), animationControl));

				animationControl->setMasterWeight(1.0f);
				animationControl->setPlaybackSpeed(1.0f);
				animationControl->easeOut(0.0f);
				animationControl->setEaseInCurve(0, 0, 1, 1);	// Smooth
				animationControl->setEaseOutCurve(1, 1, 0, 0);	// Smooth

				m_pSkeleton->addAnimationControl(animationControl);
			}
		}
	}
	else
	{
		gkLogWarning( _T("Rig [%s] load failed."), pChrRig->GetAttribute(_T("FileName")) );
	}

	// building mesh & create RenderLayer
	IGameObjectRenderLayer* pRenderLayer = gEnv->p3DEngine->createRenderLayer(m_wstrName, Vec3(0,0,0), Quat::CreateIdentity());
	if (!pRenderLayer)
	{
		GK_ASSERT(0);
		return NULL;
	}

	// load mesh

	// check if attachment
	CRapidXmlParseNode* attachmentNode = pChrMesh->getChildNode(_T("Attachment"));
	if (attachmentNode)
	{
		int submeshcount = 0;
		for (; attachmentNode; attachmentNode = attachmentNode->getNextSiblingNode(_T("Attachment")))
		{
			gkMeshPtr pMeshPtr = gEnv->pSystem->getMeshMngPtr()->load( attachmentNode->GetAttribute(_T("fileName")) );
			for (uint32 i=0; i < pMeshPtr->getSubsetCount(); ++i)
			{
				gkSkinRenderable* pRenderable = new gkSkinRenderable( pRenderLayer, this, pMeshPtr, i);
				pRenderLayer->modifySubRenderable(pRenderable, submeshcount++);
				pRenderable->setMaterialName( attachmentNode->GetAttribute(_T("MaterialName")) );
			}
		}
	}
	else
	{
		gkMeshPtr pMeshPtr = gEnv->pSystem->getMeshMngPtr()->load( pChrMesh->GetAttribute(_T("FileName")) );
        if( !pMeshPtr.isNull() )
        {
            for (uint32 i=0; i < pMeshPtr->getSubsetCount(); ++i)
            {
                gkSkinRenderable* pRenderable = new gkSkinRenderable( pRenderLayer, this, pMeshPtr, i);
                pRenderLayer->modifySubRenderable(pRenderable, i);
            }
            pRenderLayer->setMaterialName(pChrMesh->GetAttribute(_T("MaterialName")));
        }
        else
        {
            gkLogError(_T("Skinmesh: %s load failed."), pChrMesh->GetAttribute(_T("FileName")));
        }
	}

		
	getParentGameObject()->setGameObjectLayer(pRenderLayer);

	return true;
}
//////////////////////////////////////////////////////////////////////////
void gkCharacterInstance::Init()
{

}
//////////////////////////////////////////////////////////////////////////
void gkCharacterInstance::UpdateLayer( float fElpasedTime )
{
	// Advance the animation
	if (m_pSkeleton)
	{
		m_pSkeleton->stepDeltaTime( fElpasedTime );
		if (m_pPose)
		{
			m_pSkeleton->sampleAndCombineAnimations( m_pPose->accessUnsyncedPoseLocalSpace().begin(), m_pPose->getFloatSlotValues().begin()  );
		}
	}

	//DebugDraw();
	
}
//////////////////////////////////////////////////////////////////////////
void gkCharacterInstance::Destroy()
{
	if (m_pSkeleton)
	{
		m_pSkeleton->removeReference();
	}

	AnimationControlMap::iterator it = m_mapAnimations.begin();
	for (; it != m_mapAnimations.end(); ++it)
	{
		it->second->removeReference();
	}

	getAnimationPtr()->DestroyAnimLayer(m_wstrName);
}
//////////////////////////////////////////////////////////////////////////
void gkCharacterInstance::DebugDraw()
{
	// Draw
	hkQsTransform worldFromModel (hkQsTransform::IDENTITY);

	Vec3 translation = getParentGameObject()->getPosition();
	Quat rotation = getParentGameObject()->getOrientation();
	Vec3 scale = getParentGameObject()->getScale();

	worldFromModel.m_translation.set( translation.x, translation.y, translation.z );
	worldFromModel.m_rotation.set(rotation.v.x, rotation.v.y, rotation.v.z, rotation.w);
	worldFromModel.m_scale.set(scale.x, scale.y, scale.z, 1.0);

	const hkaSkeleton* skeleton = m_pPose->getSkeleton();


	const hkInt16* parents = skeleton->m_parentIndices.begin();
	const hkQsTransform* bones = m_pPose->getSyncedPoseModelSpace().begin();
	for (int i=0; i< skeleton->m_bones.getSize(); i++)
	{
		hkVector4 p1, p2;
		p1 = bones[i].getTranslation();
		if (parents[i] == -1)
		{
			p2 = p1;
		}
		else
		{
			p2 = bones[parents[i]].getTranslation();
		}

		p1.setTransformedPos(worldFromModel, p1);
		p2.setTransformedPos(worldFromModel, p2);
		Vec3 to(p1(0), p1(1), p1(2));
		Vec3 from(p2(0), p2(1), p2(2));

		ColorF whitecol = ColorF(1,1,1,1);
		gEnv->pRenderer->getAuxRenderer()->AuxRenderSkeleton(from, to, whitecol);
	}
}


void gkCharacterInstance::setAnimationLocalTime( const TCHAR* animName, float localTime )
{
	AnimationControlMap::iterator it = m_mapAnimations.find(animName);

	if (it != m_mapAnimations.end())
	{
		// found
		it->second->setLocalTime(localTime);
	}
}

//////////////////////////////////////////////////////////////////////////
void gkCharacterInstance::setAnimationSpeed( const TCHAR* animName, float speed )
{
	AnimationControlMap::iterator it = m_mapAnimations.find(animName);

	if (it != m_mapAnimations.end())
	{
		// found
		it->second->setPlaybackSpeed(speed);
	}
}

float gkCharacterInstance::getAnimationAbsTime( const TCHAR* animName )
{
	float ret = 0;
	AnimationControlMap::iterator it = m_mapAnimations.find(animName);

	if (it != m_mapAnimations.end())
	{
		// found
		//it->second->ge
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////////
void gkCharacterInstance::playAnimation( const TCHAR* animName, float blendTime )
{
	AnimationControlMap::iterator it = m_mapAnimations.find(animName);

	if (it != m_mapAnimations.end())
	{
		// found
		//it->second->setLocalTime(0);
		it->second->easeIn(blendTime);
	}
}
//////////////////////////////////////////////////////////////////////////
void gkCharacterInstance::stopAnimation( const TCHAR* animName, float blendTime )
{
	AnimationControlMap::iterator it = m_mapAnimations.find(animName);

	if (it != m_mapAnimations.end())
	{
		// found
		it->second->easeOut(blendTime);
	}
}
//////////////////////////////////////////////////////////////////////////
void gkCharacterInstance::stopAllAnimation( float blendTime )
{
	AnimationControlMap::iterator it = m_mapAnimations.begin();

	for (;it != m_mapAnimations.end(); ++it)
	{
		// found
		it->second->easeOut(blendTime);
	}
}
//////////////////////////////////////////////////////////////////////////
void gkCharacterInstance::setAnimationWeight( const TCHAR* animName, float weight )
{
	AnimationControlMap::iterator it = m_mapAnimations.find(animName);

	if (it != m_mapAnimations.end())
	{
		// found
		it->second->setMasterWeight(weight);
	}
}

int gkCharacterInstance::getAnimationCount()
{
	return m_mapAnimations.size();
}

const TCHAR* gkCharacterInstance::getAnimationName(int index)
{
	if (index >= getAnimationCount())
	{
		return NULL;
	}
	
	AnimationControlMap::iterator it = m_mapAnimations.begin();
	int size = getAnimationCount();
	for(int i=0; i < size; ++it, ++i)
	{
		if (i == index)
		{
			return it->first.c_str();
		}		
	}

	return NULL;
}

