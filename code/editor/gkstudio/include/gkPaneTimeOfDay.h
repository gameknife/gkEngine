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



#pragma once

#include "..\Resource.h"
#include "ITimeOfDay.h"
#include "gkToolBarPaneBase.h"
#include "Panels/gkPaneBase.h"
#include "Controls/gkPropertyGirdEx.h"
#include "Controls/PSCPColorDialog.h"

// gkPaneTimeOfDay dialog

class gkTODTimeLineSlider : public CSliderCtrl
{

};

class gkPaneTimeOfDay : public gkToolBarPaneBase, public IEditorUpdateListener, public IPSCPColorChangeCallback
{
	//DECLARE_DYNAMIC(gkPaneTimeOfDay)

public:
	gkPaneTimeOfDay(CWnd* pParent = NULL);   // standard constructor
	virtual ~gkPaneTimeOfDay();

// Dialog Data
	enum { IDD = IDD_TODDLG };


	CBrush m_brush;
	CStatic m_wndPlaceHolder;

	CSliderCtrl m_ctrlSlider;
	STimeOfDayKey m_curKey;
	STimeOfDayKey m_copyKey;
	//TODKeySequence m_backupSequence;


	void RefreshMetrics(BOOL bOffice2007);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	//afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	LRESULT OnGridNotify(WPARAM wParam, LPARAM lParam);
	void UpdateTodSlider();

	virtual void OnColorChange( COLORREF clr );

	gkPropertyGirdEx m_wndPropertyGrid;	

	// Generated message map functions
	//{{AFX_MSG(CPaneStyles)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();

	virtual BOOL InitPropertyTable();

	void UpdateTODKeyFromEngine();
	void UpdateTODKeyToEngine();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdrawTodslider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedTodPrevkey();
	afx_msg void OnBnClickedTodNextkey();
	afx_msg void OnBnClickedTodPlantkey();
	afx_msg void OnBnClickedTodDeletekey();

	void TodCopyKey();
	void TodPasteKey();

	void OnCommandRange(UINT);

	virtual void OnUpdate(float fElapsedTime);
	float m_edtTodTime;

	bool m_bPlaying;

public:
	virtual const TCHAR* getPanelName() {return _T("TimeOfDay Editor");}
	virtual UINT getIID() {return IDD;}
};
