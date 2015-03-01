#include "gkStableHeader.h"
#include "gkDecalsManager.h"
//#include "gkTextureManager.h"

#define DEFERRED_DECAL_TEX_MAX 4
#define DEFERRED_DECAL_MAX 30

//-----------------------------------------------------------------------
int gkDecalsManager::getDecalUnitNum()
{
	return m_vecDeferredDecals.size();
}
//-----------------------------------------------------------------------
gkDeferredDecalUnit& gkDecalsManager::getDecalUnit( int index )
{
	assert( index < getDecalUnitNum());
	return m_vecDeferredDecals[index];
}
//-----------------------------------------------------------------------
void gkDecalsManager::AddDecal( const gkStdString& textureName, Vec3 vDir, Vec3 vCenter, float fRadius /*= 5*/, float fLength /*= 0*/ )
{
	bool hasUnit = false;
	uint32 i = 0;
	for (; i < m_vecDeferredDecals.size(); ++i)
	{
		if (m_vecDeferredDecals[i].getTextureName() == textureName)
		{
			hasUnit = true;
			break;
		}
	}

	gkDecal decal(vDir, vCenter, fRadius, fLength);
	if (hasUnit)
	{
		
		m_vecDeferredDecals[i].addDecal(decal);
	}
	else
	{
		// loadit
		gEnv->pSystem->getTextureMngPtr()->load(textureName);
		// build it
		gkDeferredDecalUnit unit(textureName);
		m_vecDeferredDecals.push_back(textureName);
		m_vecDeferredDecals.back().addDecal(decal);
	}

	m_bCacheChanged = true;
}
//-----------------------------------------------------------------------
const gkStdString& gkDecalsManager::getDecalTextureName( int unitIndx )
{
	assert( unitIndx < getDecalUnitNum());
	return m_vecDeferredDecals[unitIndx].getTextureName();
}
//-----------------------------------------------------------------------
int* gkDecalsManager::getDeferredDecalTextureDivide()
{
	return m_aTexDivide;
}
//-----------------------------------------------------------------------
Vec4* gkDecalsManager::getDeferredDecalDataCache()
{
	if (m_bCacheChanged)
	{
		m_vecDefferredDecalDataCache.clear();
		int nTexCount = 0;
		for (uint32 i=0; i< m_vecDeferredDecals.size(); ++i)
		{
			Vec4* start = m_vecDeferredDecals[i].getDecalsDataHandle();
			for (int j =0; j< m_vecDeferredDecals[i].getDecalNum() * 2; ++j)
			{
				m_vecDefferredDecalDataCache.push_back(start[j]);
			}
			nTexCount += m_vecDeferredDecals[i].getDecalNum();
			if (i < DEFERRED_DECAL_TEX_MAX)
			{
				m_aTexDivide[i] = nTexCount;
			}
		}
		m_bCacheChanged = false;
	}	

	return &(m_vecDefferredDecalDataCache[0]);
}
//-----------------------------------------------------------------------
int gkDecalsManager::getDeferredDecalNum()
{
	if (m_bCacheChanged)
	{
		getDeferredDecalDataCache();
	}


	int nCount = m_vecDefferredDecalDataCache.size() / 2;
	if(nCount < DEFERRED_DECAL_MAX)
	{
		return nCount;
	}
	else
	{
		return DEFERRED_DECAL_MAX;
	}
}
//-----------------------------------------------------------------------
gkDecalsManager::gkDecalsManager()
{
	m_aTexDivide[0] = DEFERRED_DECAL_MAX;
	m_aTexDivide[1] = DEFERRED_DECAL_MAX;
	m_aTexDivide[2] = DEFERRED_DECAL_MAX;

	m_bCacheChanged = false;
}
//-----------------------------------------------------------------------
gkDecalsManager::~gkDecalsManager()
{

}