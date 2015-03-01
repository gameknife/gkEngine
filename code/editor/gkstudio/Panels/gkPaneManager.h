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



#include "DockingPane\XTPDockingPaneManager.h"
//////////////////////////////////////////////////////////////////////////
//
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	gkPaneManager.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/4/4
// Modify:	2012/4/4
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _GKPANEMANAGER_H_
#define _GKPANEMANAGER_H_

class IPaneBase;

enum EPaneType
{
	ePT_PaneBase,
	ePT_Ctrl,
	ePT_View,
};

struct PanelInfo 
{
	uint32 m_paneID;
	EPaneType m_paneType;
	CWnd* m_paneTarget;
};

class CXTPDockingPaneVisualStudio2010CustomTheme : public CXTPDockingPaneVisualStudio2010Theme
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of manager.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();
	void DrawFloatingFrame(CDC* pDC, CXTPDockingPaneMiniWnd* pPane, CRect rc);
	virtual COLORREF FillCaptionPart(CDC* pDC, CXTPDockingPaneBase* pPane, CRect rcCaption, BOOL bActive, BOOL bVertical);
};

class gkPaneManager : public CXTPDockingPaneManager
{
public:
	gkPaneManager();
	virtual ~gkPaneManager() {}

	void Init(CWnd* parent);
	void Destroy();
	void RegisterPanelDialog(IPaneBase* panel);
	void RegisterPanelManmual(CWnd* ctrl, const TCHAR* name);
	// link registered pane to dockingpane
	void LinkPanel(const TCHAR* name);

	// open dockingpane
	void OpenPanel(const TCHAR* name);

	// close dockingpane
	void ClosePanel(const TCHAR* name);

	// open/close dockingpane
	void SwitchPanel(const TCHAR* name);

	// check if dockingpane open
	bool IsPanelOpen(const TCHAR* name);

	// destroy dockingpane
	void DestroyPanel(const TCHAR* name);

	void RefreshMetrics(BOOL bOffice2007);

private:

	typedef std::map<const TCHAR*, PanelInfo> PanelMap;
	PanelMap m_mapPanels;

	uint32 m_nPanelIDCounter;
};

#endif