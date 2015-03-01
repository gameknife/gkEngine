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
// Name:   	gkSceneQuery.h
// Desc:	scene query	
// 
// Author:  Kaiming-Desktop
// Date:	2011 /8/22
// Modify:	2011 /8/22
// 
//////////////////////////////////////////////////////////////////////////

#ifndef GKSCENEQUERY_H_
#define GKSCENEQUERY_H_

#include "gkPrerequisites.h"
#include "gk_Geo.h"

class gkSceneQuery
    {
    public:

    protected:
        gkSceneManager* m_pParentSceneMgr;
        uint32 m_uQueryMask;
		//uint32 m_uQueryTypeMask;
  
    public:
        /** Standard constructor, should be called by SceneManager. */
		gkSceneQuery(gkSceneManager* mgr):m_pParentSceneMgr(mgr)
			,m_uQueryMask(0)
		{}
		virtual ~gkSceneQuery(){}

		virtual void setQueryMask(uint32 mask) {m_uQueryMask = mask;}
        /** Returns the current mask for this query. */
		virtual uint32 getQueryMask(void) const {return m_uQueryMask;}
// 
//         virtual void setQueryTypeMask(uint32 mask);
//         /** Returns the current mask for this query. */
//         virtual uint32 getQueryTypeMask(void) const;
//         
    };

/** This struct allows a single comparison of result data no matter what the type */
struct gkRaySceneQueryResultEntry
{
	/// Distance along the ray
	f32 distance;
	/// The movable, or NULL if this is not a movable result
	gkMovableObject* movable;

	/// Comparison operator for sorting
	bool operator < (const gkRaySceneQueryResultEntry& rhs) const
	{
		return this->distance < rhs.distance;
	}
};

typedef std::vector<gkRaySceneQueryResultEntry> gkRaySceneQueryResult;


 class gkRaySceneQuery : public gkSceneQuery
    {
    protected:
        Ray m_vRay;
        bool m_bSortByDistance;
        uint8 m_uMaxResults;
        gkRaySceneQueryResult m_Result;

    public:
        gkRaySceneQuery(gkSceneManager* mgr);
        virtual ~gkRaySceneQuery();

		virtual void setRay(const Ray& ray) {m_vRay = ray;}
		
		virtual const Ray& getRay(void) const {return m_vRay;}

        virtual void setSortByDistance(bool sort, uint8 maxresults = 0);
        /** Gets whether the results are sorted by distance. */
		virtual bool getSortByDistance(void) const {return m_bSortByDistance;}
        /** Gets the maximum number of results returned from the query (only relevant if 
        results are being sorted) */
		virtual uint8 getMaxResults(void) const {return m_uMaxResults;}

        virtual gkRaySceneQueryResult& execute(void);

        virtual gkRaySceneQueryResult& getLastResults(void);
        /** Clears the results of the last query execution.
        @remarks
            You only need to call this if you specifically want to free up the memory
            used by this object to hold the last query results. This object clears the
            results itself when executing and when destroying itself.
        */
        virtual void clearResults(void);

        /** Self-callback in order to deal with execute which returns collection. */
        bool queryResult(gkMovableObject* obj, f32 distance);
    };



#endif

