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
// Name:   	TestCaseFramework.h
// Desc:	
// 	
// 
// Author:  Kaiming
// Date:	2013/3/27	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef TestCaseFramework_h__
#define TestCaseFramework_h__

#include "IGame.h"
#include "IInputManager.h"
#include "IGameObject.h"
#include "IGameObjectLayer.h"
#include "ITimer.h"
#include "IRenderer.h"

enum ETestCaseCategory
{
	eTcc_Loading = 0,
	eTcc_Animation,
	eTcc_Rendering,
	eTcc_Performance,
    eTcc_System,

	eTcc_Count,
};

enum ETestCaseStatus
{
	eTS_running,
	eTS_pausing,
	eTS_stop,
};
struct TestCaseBase
{
	TestCaseBase():m_status(eTS_stop) {}
	virtual ~TestCaseBase() {}

	void Init()
	{
		gkLogInput( _T("TestCase [%s] Start."), GetName() );	
		OnInit();
		m_status = eTS_running;
	}

	bool Update()
	{
		return OnUpdate();
	}

	void Destroy()
	{
		OnDestroy();
		m_status = eTS_stop;
		gkLogInput( _T("TestCase [%s] Stop."), GetName() );	
	}

	void InputEvent( const SInputEvent &event )
	{
		OnInputEvent(event);
	}

	virtual const TCHAR* GetName() =0;
	ETestCaseStatus m_status;


protected:
	virtual void OnInit() =0;
	//************************************
	// Method:    OnUpdate
	// FullName:  TestCaseBase::OnUpdate
	// Access:    virtual public 
	// Returns:   bool 需要退出返回false, 否则返回true
	// Qualifier:
	//************************************
	virtual bool OnUpdate() =0;
	virtual void OnDestroy() =0;
	virtual void OnInputEvent( const SInputEvent &event ) =0;
	
};

typedef std::vector<TestCaseBase*> TestCases;
typedef std::vector<RectF> TestCasesButtons;
typedef std::map<uint32, TestCases> TestCasesMap;
extern TestCases g_cateTestCases[eTcc_Count];

class TestCaseFramework : 
	public IGame, 
	public IInputEventListener
{
public:
	TestCaseFramework(void);
	~TestCaseFramework(void);

	virtual bool OnInit();

	virtual bool OnDestroy();

	virtual bool OnUpdate();

	virtual bool OnLevelLoaded();

	virtual bool OnInputEvent( const SInputEvent &event );


private:

	void OnTouch(const Vec2& touchpos);

	void UpdateGUI(bool menuMode);

	void RestoreSetting();
    
    void IntoSubSection();
    void IntoMainSection();


	struct IFtFont* m_mainFont;
	struct IFtFont* m_menuFont;
	uint32 m_testCase_Point;
	uint32 m_testCase_sub_Point;

	TestCaseBase* m_runningCase;
	TestCasesButtons m_buttons;
	TestCasesButtons m_section_buttons;

	int32 m_widget;
};

#endif



