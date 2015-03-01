#include "RendererD3D9StableHeader.h"
#include "gkGPUTimer.h"


gkGpuTimer::gkGpuTimer()
{
	m_pEventStart = NULL;
	m_pEventStop = NULL;
	m_pEventFreq = NULL;
	m_wstrName = _T("undefine");
	m_fElapsedTime = 0.0f;

	m_skip = true;

	for (int i=0; i < 20; ++i)
	{
		m_tmpTime[i] = 0;
	}
	
	m_recorder = 0;
}

gkGpuTimer::gkGpuTimer( const TCHAR* name )
{
	m_pEventStart = NULL;
	m_pEventStop = NULL;
	m_pEventFreq = NULL;
	m_wstrName = name;
	m_fElapsedTime = 0.0f;

	m_skip = true;

	for (int i=0; i < 20; ++i)
	{
		m_tmpTime[i] = 0;
	}

	m_recorder = 0;
}

void gkGpuTimer::init(IDirect3DDevice9* pDevice)
{
	// initialze the querys [2/2/2012 Kaiming]
	pDevice->CreateQuery(D3DQUERYTYPE_TIMESTAMP, &m_pEventStart);
	pDevice->CreateQuery(D3DQUERYTYPE_TIMESTAMP, &m_pEventStop);
	pDevice->CreateQuery(D3DQUERYTYPE_TIMESTAMPFREQ, &m_pEventFreq);
}

void gkGpuTimer::destroy()
{
	SAFE_RELEASE( m_pEventStart );
	SAFE_RELEASE( m_pEventStop );
	SAFE_RELEASE( m_pEventFreq );
}

void gkGpuTimer::start()
{
	if( g_pRendererCVars->r_ProfileGpu == 1 )
	{
		// get start time stamp and freq
		if (m_pEventStart)
			m_pEventStart->Issue(D3DISSUE_END);
		if (m_pEventFreq)
			m_pEventFreq->Issue(D3DISSUE_END);
	}

	m_skip = false;
}

void gkGpuTimer::stop()
{
	if( g_pRendererCVars->r_ProfileGpu == 1 )
	{
		// get end time stamp
		if (m_pEventStop)
			m_pEventStop->Issue(D3DISSUE_END);
	}

	m_skip = false;
}

#define GKGPUTIMER_TRYCOUNT 10000

void gkGpuTimer::update()
{
	if( g_pRendererCVars->r_ProfileGpu == 1 )
	{
		UINT64 startTime = 0;
		UINT64 endTime = 0;
		UINT64 freq = 1;

		if (gEnv->pProfiler->getFrameCount() % 2 == 0)
		{
			if (!m_skip)
			{
				// at the end of a frame, getdata from device
				int trycount = 0;
				bool failed = 0;
				if(m_pEventStart && m_pEventStop && m_pEventFreq)
				{
					while(trycount < GKGPUTIMER_TRYCOUNT && S_FALSE == m_pEventStart->GetData( (void *)&startTime, sizeof(UINT64), D3DGETDATA_FLUSH ))
					{
						trycount++;
					}

					if (trycount == GKGPUTIMER_TRYCOUNT)
					{
						failed = 1;
					}
					trycount = 0;

					while(trycount < GKGPUTIMER_TRYCOUNT && S_FALSE == m_pEventStop->GetData( (void *)&endTime, sizeof(UINT64), D3DGETDATA_FLUSH ))
					{
						trycount++;
					}

					if (trycount == GKGPUTIMER_TRYCOUNT)
					{
						failed = 1;
					}
					trycount = 0;

					while(trycount < GKGPUTIMER_TRYCOUNT && S_FALSE == m_pEventFreq->GetData( (void *)&freq, sizeof(UINT64), D3DGETDATA_FLUSH ))
					{
						trycount++;
					}

					if (trycount == GKGPUTIMER_TRYCOUNT)
					{
						failed = 1;
					}
					trycount = 0;
				}




				// calculate the elapsedtime in microseconds
				if(!failed)
				{
					m_fElapsedTime = (float)(endTime - startTime) / (float)(freq) * 1000.0f;
				}

				m_tmpTime[m_recorder] = m_fElapsedTime;

				float sumTime = 0;
				for (int i=0; i < 20; ++i)
				{
					sumTime += m_tmpTime[i];
				}
				m_fAvgElapsedTime = sumTime / 20.0f;


				m_recorder++;
				m_recorder %= 20;
			}
			else
			{
				m_fElapsedTime = 0.0f;
			}
		}
		

		
	}
	m_skip = true;
}

