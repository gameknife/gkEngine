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
// VR Platform (C) 2013
// OpenVRP Kernel Source File 
//
// Name:   	PSCPColorDialog.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/10/10
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _PSCPColorDialog_h_
#define _PSCPColorDialog_h_

#define XUI_WM_COLOR_CHANGE (WM_USER + 8000)

struct IPSCPColorChangeCallback
{
	virtual void OnColorChange( COLORREF clr ) =0;
};

typedef std::vector<IPSCPColorChangeCallback*> IPSCPColorChangeCallbacks;

class CPSCPColorDialog : public CXTPColorDialog
{
public:
	CPSCPColorDialog( COLORREF clrNew, COLORREF clrCurrent, DWORD dwFlags = 0L, CWnd* pWndParent = NULL );
	~CPSCPColorDialog(void);

	virtual BOOL OnInitDialog();

	void addListener(IPSCPColorChangeCallback* list) {m_listener.push_back(list);}
	IPSCPColorChangeCallbacks& getListener() {return m_listener;}

	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTPColorDialog)
	afx_msg void OnPaint();


protected:
	IPSCPColorChangeCallbacks m_listener;
};


#endif

