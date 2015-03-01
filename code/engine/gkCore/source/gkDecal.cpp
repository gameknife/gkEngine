#include "gkStableHeader.h"
#include "gkDecal.h"

//-----------------------------------------------------------------------
gkDecal::gkDecal( Vec3 vDir, Vec3 vCenter, float fRadius /*= 5*/, float fLength /*= 0 */ ):m_vDirection(vDir)
,m_vCenter(vCenter)
,m_fRadius(fRadius)
,m_fLength(fLength)
{

}
	//-----------------------------------------------------------------------
gkDecal::~gkDecal()
{

}
//-----------------------------------------------------------------------
Vec4 gkDecal::getDirectionVector()
{
	return Vec4(m_vDirection.x, m_vDirection.y, m_vDirection.z, m_fLength);
}
//-----------------------------------------------------------------------
Vec4 gkDecal::getCenterVector()
{
	return Vec4(m_vCenter.x, m_vCenter.y, m_vCenter.z, m_fRadius);
}
//-----------------------------------------------------------------------
void gkDecal::setDirection(Vec3& vDir)
{
	m_vDirection = vDir;
}
//-----------------------------------------------------------------------
void gkDecal::setCenter(Vec3& vCenter)
{
	m_vCenter = vCenter;
}
//-----------------------------------------------------------------------
gkDeferredDecalUnit::gkDeferredDecalUnit( gkStdString wstrName ):m_wstrName(wstrName)
{
	m_vecDecalList.clear();
	m_vecCachedData.clear();
}
//-----------------------------------------------------------------------
gkDeferredDecalUnit::~gkDeferredDecalUnit()
{
}
//-----------------------------------------------------------------------
void gkDeferredDecalUnit::addDecal( gkDecal pDecal )
{
	m_vecDecalList.push_back(pDecal);
	m_vecCachedData.push_back(pDecal.getDirectionVector());
	m_vecCachedData.push_back(pDecal.getCenterVector());
}
//-----------------------------------------------------------------------
Vec4* gkDeferredDecalUnit::getDecalsDataHandle()
{
	return &(m_vecCachedData[0]);
}
//-----------------------------------------------------------------------
int gkDeferredDecalUnit::getDecalNum()
{
	return m_vecDecalList.size();
}
//-----------------------------------------------------------------------
const gkStdString& gkDeferredDecalUnit::getTextureName()
{
	return m_wstrName;
}