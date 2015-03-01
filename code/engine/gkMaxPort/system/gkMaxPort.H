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



/**********************************************************************
 *<
	FILE: prim.h

	DESCRIPTION:

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __PRIM__H
#define __PRIM__H

#include "Max.h"
#include "../resource.h"
#include "../resource1.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "decomp.h"

#include "ISystem.h"

TCHAR *GetString(int id);

#define IGAMEEXPORTER_CLASS_ID		Class_ID(0x79d613a4, 0x4f21c3ad)
#define OBJEXPORTER_CLASSID			Class_ID(1407730388, 635136974)		

extern ClassDesc* GetgkObjectInMaxDesc();
extern ClassDesc2* GetIGameExporterDesc();

class IGameExporter;
extern IGameExporter* gExporter;

struct gkUtilityGlobalEnv
{
	HINSTANCE hInst;
	TSTR SyncSceneName;
	ISystem* pSystem;
	bool m_bUpdate;
	bool m_bMaxPorted;

	gkUtilityGlobalEnv()
	{
		hInst = NULL;
		SyncSceneName = "Default.gks";
		pSystem = NULL;
		m_bUpdate = false;
		m_bMaxPorted = false;
	}

	void Init();

	void Destroy();


};

template<class T>
struct MaxObjVertexRef
{
	std::vector<DWORD> normalIdx;		// vertex 2 normidx
	std::vector<T> m_vecVectors;

	int vtxCounter;
	float mprecision;

	MaxObjVertexRef()
	{
		m_vecVectors.clear();
		vtxCounter = 0;
		mprecision = 0.01f;
	}

	void AddNormalized(T& norm)
	{
		norm = norm.Normalize();
		Add( norm );
	}

	void Add(T& norm)
	{
		int thisIdx = 0;

		for (int i=0; i< m_vecVectors.size(); ++i)
		{
			if ((m_vecVectors[i] - norm).Length() < mprecision)
			{
				// exist
				thisIdx = i + 1;
				break;
			}
			else
			{

			}
		}
		if (thisIdx == 0)
		{
			// new norm [9/3/2011 Kaiming-Desktop]
			m_vecVectors.push_back(norm);
			thisIdx = m_vecVectors.size();
		}
		normalIdx.push_back( thisIdx );
	}

	void SetPrecision(float precision) {mprecision = precision;}
};

extern gkUtilityGlobalEnv* gUtilEnv;

extern HINSTANCE hInstance;

void getEngineRoot(TCHAR* path);


#endif
