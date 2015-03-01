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
// Name:   	ColorPageAdvance.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/10/10
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _ColorPageAdvance_h_
#define _ColorPageAdvance_h_

#include "resource.h"

class CXTPColorDialog;

enum COLOR_MODE
{
	MODE_HUE,
	MODE_SAT,
	MODE_BRIGHTNESS,

	MODE_RED,
	MODE_GREEN,
	MODE_BLUE,
};

//===========================================================================
// Summary:
//     CXTPColorBase is a CStatic derived base class. It is used to create
//     custom color selection windows.
//===========================================================================
class CPSCPColorBase : public CStatic
{
public:
	// ----------------------------------------------------------------------
	// Summary:
	//     Enumeration used to determine window focus.
	// Remarks:
	//     FocusControl type defines the constants used by CXTPColorBase for
	//     determining which area of the color window control has focus.
	// See Also:
	//     CXTPColorBase
	//
	// <KEYWORDS focusNone, focusColorWheel, focusLumination>
	// ----------------------------------------------------------------------
	enum FocusedControl
	{
		focusNone,       // Color window does not have input focus.
		focusColorWheel, // Color wheel window has focus.
		focusLumination  // Lumination window has focus.
	};

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPColorBase object
	//-----------------------------------------------------------------------
	CPSCPColorBase();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPColorBase object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CPSCPColorBase();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to update the cursor position.
	// Parameters:
	//     point   - Current cursor location relative to the device context.
	//-----------------------------------------------------------------------
	virtual void UpdateCursorPos(CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to convert an RGB color value to
	//     an HSL value.
	// Parameters:
	//     color - An RGB value that represents the value to convert to HSL.
	//     h     - Represents the color hue.
	//     s     - Represents the color saturation.
	//     l     - Represents the color illumination.
	//-----------------------------------------------------------------------
	static void RGBtoHSL(COLORREF color, double &h, double &s, double& l);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to convert an HLS value to an RGB
	//     color value.
	// Parameters:
	//     h - Represents the color hue.
	//     l - Represents the color illumination.
	//     s - Represents the color saturation.
	// Returns:
	//     A COLORREF value.
	//-----------------------------------------------------------------------
	static COLORREF HLStoRGB(double h, double l, double s);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the CXTPColorBase class to
	//     perform initialization when the window is created or sub-classed.
	// Returns:
	//     TRUE if the window was successfully initialized, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual bool Init();

//{{AFX_CODEJOCK_PRIVATE

	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPColorBase)
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPColorBase)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	static
	FocusedControl m_eHasFocus;        // Determines which property page has input focus.
	bool           m_bPreSubclassInit; // true when initializing from PreSubclassWindow.

public:


	double m_nLum;       // Current illumination value.
	double m_nSat;       // Current saturation value.
	double m_nHue;       // Current hue value.

	int m_nRed;
	int m_nGreen;
	int m_nBlue;

	DWORD m_mode;

	bool m_needUpdate;

	CPoint m_ptMousePos; // Current mouse position relative to the device context.
};

//===========================================================================
// Summary:
//     CXTPColorWnd is a CXTPColorBase derived class. It is used to create a
//     color selection window.
//===========================================================================
class CPSCPColorWnd : public CPSCPColorBase
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPColorWnd object
	//-----------------------------------------------------------------------
	CPSCPColorWnd();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPColorWnd object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CPSCPColorWnd();

public:

	COLORREF TRANSCODE_RGB(int x, int y);
	COLORREF GET_RGB_BY_CURSOR();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the color for the selection window.
	// Parameters:
	//     nHue     - Current hue value that represents the color.
	//     nSat     - Current saturation value that represents the color.
	//-----------------------------------------------------------------------
	virtual void SetColor(COLORREF color);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to update the cursor position.
	// Parameters:
	//     point   - Current cursor location relative to the device context.
	//-----------------------------------------------------------------------
	virtual void UpdateCursorPos(CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to update the cross hair cursor.
	// Parameters:
	//     pDC - A CDC pointer that represents the current device context.
	//-----------------------------------------------------------------------
	void DrawCrossHair(CDC* pDC);

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPColorWnd)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPColorWnd)
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	CBitmap        m_bmpPicker;         // Background device context.
};

//===========================================================================
// Summary:
//     CXTPColorLum is a CXTPColorBase derived class. It is used to create a
//     color luminance selection bar.
//===========================================================================
class CPSCPColorLum : public CPSCPColorBase
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPColorLum object
	//-----------------------------------------------------------------------
	CPSCPColorLum();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPColorLum object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CPSCPColorLum();

public:

	COLORREF GetColorFromX(double digit);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the color for the selection window.
	// Parameters:
	//     nHue     - Current hue value that represents the color.
	//     nSat     - Current saturation value that represents the color.
	//-----------------------------------------------------------------------
	virtual void SetColor(COLORREF color);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to update the cursor position.
	// Parameters:
	//     point   - Current cursor location relative to the device context.
	//-----------------------------------------------------------------------
	virtual void UpdateCursorPos(CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to draw the indicator arrow.
	// Parameters:
	//     pDC - A CDC pointer that represents the current device context.
	//-----------------------------------------------------------------------
	void DrawSliderArrow(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to draw the selection bar.
	// Parameters:
	//     pDC - A CDC pointer that represents the current device context.
	//-----------------------------------------------------------------------
	void DrawLuminanceBar(CDC* pDC);

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function is called to retrieve the display size of the
	//     luminance bar.
	// Parameters:
	//     rect -  Address to a CRect object.
	// ----------------------------------------------------------------------
	void GetLumBarRect(CRect& rect);

protected:

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPColorLum)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPColorLum)
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

	COLORREF m_clr;

protected:
};

class CPSCPColorDialog;
//===========================================================================
// Summary:
//     CXTPColorPageCustom is derived from CPropertyPage. It is used
//     to create a CXTPColorPageCustom dialog.
//===========================================================================
class CPSCPColorPageCustom : public CPropertyPage
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPColorPageCustom object
	// Parameters:
	//     pParentSheet - A pointer to the parent sheet.
	//-----------------------------------------------------------------------
	CPSCPColorPageCustom(CPSCPColorDialog* pParentSheet = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPColorPageCustom object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CPSCPColorPageCustom();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to retrieve the HSL values of the RGB
	//     specified by color.
	// Parameters:
	//     color - An RGB value that represents the color.
	//     xtFocusLumination   - Address of the integer to receive the illumination value (0-255).
	//     sat   - Address of the integer to receive the saturation value (0-255).
	//     hue   - Address of the integer to receive the hue value (0-255).
	//-----------------------------------------------------------------------
	void RGBtoHSL(COLORREF color, double& h, double& s, double& l);
	void RGBtoHSL(COLORREF color, int& h, int& s, int& l);
protected:

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPColorPageCustom)
	virtual BOOL OnSetActive();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPColorPageCustom)
	void UpdateRGB(COLORREF clr);
	afx_msg void OnChangeEdit();

	afx_msg void OnChangeModeH() {ChangeMode(MODE_HUE);}
	afx_msg void OnChangeModeS() {ChangeMode(MODE_SAT);}
	afx_msg void OnChangeModeL() {ChangeMode(MODE_BRIGHTNESS);}
	afx_msg void OnChangeModeR() {ChangeMode(MODE_RED);}
	afx_msg void OnChangeModeG() {ChangeMode(MODE_GREEN);}
	afx_msg void OnChangeModeB() {ChangeMode(MODE_BLUE);}

	afx_msg void OnChangeEditHLS();

	afx_msg LRESULT OnUpdateColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnColorDblClick(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

	void ChangeMode( DWORD mode );

protected:
//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_DATA(CXTPColorPageCustom)
	enum { IDD = IDD_PSCP };
	CPSCPColorWnd m_colorWnd;
	CPSCPColorLum m_colorLum;

	CXTPEdit m_editHue;
	CXTPEdit m_editSat;
	CXTPEdit m_editLum;

	CXTPEdit m_editGreen;
	CXTPEdit m_editBlue;
	CXTPEdit m_editRed;

	CXTPEdit m_editColorRef;

	int m_useR;
	int m_useG;
	int m_useB;

	int m_useH;
	int m_useS;
	int m_useL;

	int     m_nR;
	int     m_nB;
	int     m_nG;
	int     m_nH;
	int     m_nL;
	int     m_nS;
	CString	m_colorref;

	DWORD m_mode;
	//}}AFX_DATA
//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPColorDialog*     m_pParentSheet; // Points to the parent property sheet
	friend class CPSCPColorLum;
};

#endif // !defined(__XTPCOLORCUSTOM_H__)
