#include "TestCaseFramework.h"
#include "TestCase_Template.h"
#include "ITerrianSystem.h"
#include "I3DEngine.h"
#include "ICamera.h"
#include "TestCaseUtil_Cam.h"
#include "IGameFramework.h"
#include "ITimeOfDay.h"

#include "IEvent.h"

TEST_CASE_FASTIMPL_HEAD( TestCase_ThreadTest, eTcc_System )

	;

class CTestThread : public IThread
{
public:
	virtual int Run() 
	{
		while(1)
		{
			m_signal.Wait();

			gkLogMessage( _T("Thread running.") );
			gkSleep(500);
			gkLogMessage( _T("Thread finish.") );

			m_signal.Reset();
		}



		return 0;
	}
    

    CManualResetEvent m_signal;

};

CTestThread* thread;

virtual void OnInit() 
{
	thread = new CTestThread;

	thread->Start();
    
    gkLogMessage( _T("Thread started, but waiting.") );
}

virtual bool OnUpdate() 
{
	if( gEnv->pInGUI->gkGUIButton( _T("signal"), Vec2(gEnv->pRenderer->GetScreenWidth() / 2 - 100, 100), 200, 60, ColorB(255,255,255,255), ColorB(0,0,0,128)  ) )
	{
		thread->m_signal.Set();
	}

	return true;
}

virtual void OnDestroy() 
{

}

virtual void OnInputEvent( const SInputEvent &event ) 
{
	HandleFreeCam( event );
	//HandleModelViewCam(event, 0.002f);
}

TEST_CASE_FASTIMPL_TILE( TestCase_ThreadTest )