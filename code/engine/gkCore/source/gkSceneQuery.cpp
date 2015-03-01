#include "gkStableHeader.h"
#include "gkSceneQuery.h"

//-----------------------------------------------------------------------
gkRaySceneQuery::gkRaySceneQuery( gkSceneManager* mgr ):gkSceneQuery(mgr)
	,m_bSortByDistance(false)
	,m_uMaxResults(0)
{

}
//-----------------------------------------------------------------------
gkRaySceneQuery::~gkRaySceneQuery()
{

}
//-----------------------------------------------------------------------
void gkRaySceneQuery::setSortByDistance( bool sort, uint8 maxresults /*= 0*/ )
{
	m_bSortByDistance = sort;
	m_uMaxResults = maxresults;
}
//-----------------------------------------------------------------------
gkRaySceneQueryResult& gkRaySceneQuery::execute( void )
{
	// Clear without freeing the vector buffer
	//m_Result.clear();

	if (m_bSortByDistance)
	{
		if (m_uMaxResults != 0 && m_uMaxResults < m_Result.size())
		{
			// Partially sort the N smallest elements, discard others
			std::partial_sort(m_Result.begin(), m_Result.begin()+m_uMaxResults, m_Result.end());
			m_Result.resize(m_uMaxResults);
		}
		else
		{
			// Sort entire result array
			std::sort(m_Result.begin(), m_Result.end());
		}
	}

	return m_Result;
}
//-----------------------------------------------------------------------
gkRaySceneQueryResult& gkRaySceneQuery::getLastResults( void )
{
	return m_Result;
}
//-----------------------------------------------------------------------
void gkRaySceneQuery::clearResults( void )
{
	// C++ idiom to free vector buffer: swap with empty vector
	gkRaySceneQueryResult().swap(m_Result);
}
//-----------------------------------------------------------------------
bool gkRaySceneQuery::queryResult( gkMovableObject* obj, f32 distance )
{
	// Add to internal list
	gkRaySceneQueryResultEntry dets;
	dets.distance = distance;
	dets.movable = obj;
	m_Result.push_back(dets);
	// Continue
	return true;
}

