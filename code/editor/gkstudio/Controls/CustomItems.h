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



// Custom Items.h : header file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2009 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "PSCPColorDialog.h"
typedef void ( * COLORCHANGE_CALLBACK)(COLORREF clr);

class CCustomItemIcon : public CXTPPropertyGridItem
{
public:
	CCustomItemIcon(CString strCaption, HICON hIcon = 0);

	~CCustomItemIcon(void);

protected:
	virtual BOOL OnDrawItemValue(CDC& dc, CRect rcValue);
	virtual void OnInplaceButtonDown(CXTPPropertyGridInplaceButton* pButton);

private:
	HICON m_hIcon;
};

class CCustomItemSpin;

class CCustomItemSpinInplaceButton : public CSpinButtonCtrl
{
	friend class CCustomItemSpin;
	CCustomItemSpin* m_pItem;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDeltapos(NMHDR *pNMHDR, LRESULT *pResult);
};

class CCustomItemSpin : public CXTPPropertyGridItemNumber
{
	friend class CCustomItemSpinInplaceButton;
public:
	CCustomItemSpin(CString strCaption);


protected:
	virtual void OnDeselect();
	virtual void OnSelect();
	virtual CRect GetValueRect();


private:
	CCustomItemSpinInplaceButton m_wndSpin;
};

class CCustomItemEdit : public CXTPPropertyGridItem
{
public:
	CCustomItemEdit(CString strCaption, CString strValue)
		: CXTPPropertyGridItem(strCaption, strValue)
	{
		m_nFlags = xtpGridItemHasComboButton|xtpGridItemHasEdit;
		GetConstraints()->AddConstraint(_T("<Edit...>"));
	}

protected:
	virtual void OnValueChanged(CString strValue) 
	{
		if (strValue == _T("<Edit...>"))
		{
			AfxMessageBox(_T("Show Edit Dialog Box"));
		}
		else
		{
			CXTPPropertyGridItem::OnValueChanged(strValue);

		}
	}

};

class CCustomItemFileBox : public CXTPPropertyGridItem
{
public:
	CCustomItemFileBox(CString strCaption);


protected:
	virtual void OnInplaceButtonDown(CXTPPropertyGridInplaceButton* pButton);
	//virtual void On
};

class CCustomItemFilename : public CXTPPropertyGridItem
{
public:
	CCustomItemFilename(CString strCaption);


protected:
	virtual void OnInplaceButtonDown(CXTPPropertyGridInplaceButton* pButton);
};

class CCustomItemCheckBox;

class CInplaceCheckBox : public CButton
{
public:
	afx_msg LRESULT OnCheck(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT /*nCtlColor*/);

	DECLARE_MESSAGE_MAP()
protected:
	CCustomItemCheckBox* m_pItem;
	COLORREF m_clrBack;
	CBrush m_brBack;

	friend class CCustomItemCheckBox;
};

class CCustomItemCheckBox : public CXTPPropertyGridItem
{
protected:

public:
	CCustomItemCheckBox(CString strCaption);

	BOOL GetBool();
	void SetBool(BOOL bValue);

protected:
	virtual void OnDeselect();
	virtual void OnSelect();
	virtual CRect GetValueRect();
	virtual BOOL OnDrawItemValue(CDC& dc, CRect rcValue);

	virtual BOOL IsValueChanged();




private:
	CInplaceCheckBox m_wndCheckBox;
	BOOL m_bValue;

	friend class CInplaceCheckBox;
};



class CCustomItemChilds : public CXTPPropertyGridItem
{
	class CCustomItemChildsAll;
	class CCustomItemChildsPad;

	friend class CCustomItemChildsAll;
	friend class CCustomItemChildsPad;

public:
	CCustomItemChilds(CString strCaption, CRect rcValue);

protected:
	virtual void OnAddChildItem();
	virtual void SetValue(CString strValue);

private:
	void UpdateChilds();
	CString RectToString(CRect rc);


private:
	CCustomItemChildsAll* m_itemAll;
	CCustomItemChildsPad* m_itemLeft;
	CCustomItemChildsPad* m_itemTop;
	CCustomItemChildsPad* m_itemRight;
	CCustomItemChildsPad* m_itemBottom;

	CRect m_rcValue;
};

class CCustomItemColor: public CXTPPropertyGridItemColor, public IPSCPColorChangeCallback
{
public:
	CCustomItemColor(CString strCaption, IPSCPColorChangeCallback* listener, COLORREF clr = 0);

// protected:
 	virtual void OnInplaceButtonDown(CXTPPropertyGridInplaceButton* pButton);

#ifndef _DECIMAL_VALUE
public:

	void SetValue(CString strValue)
	{
		SetColor(StringToRGB(strValue));
	}

	void SetColor(COLORREF clr)
	{
		m_clrValue = clr;
		CXTPPropertyGridItem::SetValue(RGBToString(clr));
		if (m_colorf)
		{
			m_colorf[0] = (float)GetRValue(GetColor()) / 255.0f;
			m_colorf[1] = (float)GetGValue(GetColor()) / 255.0f;
			m_colorf[2] = (float)GetBValue(GetColor()) / 255.0f;
		}
	}

	void SetColor(float* color)
	{
		// fatal
		int r = color[0] * 255;
		int g = color[1] * 255;
		int b = color[2] * 255;

		SetColor(RGB(r,g,b));
	}



	void BindToColorF(float* colorR)
	{
		m_colorf = colorR;
	}

	virtual void OnBeforeInsert();

	virtual void OnColorChange( COLORREF clr );

private:
	float* m_colorf;
	IPSCPColorChangeCallback* m_listener;
#endif
};




//////////////////////////////////////////////////////////////////////////


class CInplaceUpperCase : public CXTPPropertyGridInplaceEdit
{
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_DYNAMIC(CInplaceUpperCase);
	DECLARE_MESSAGE_MAP()
};


class CCustomItemUpperCase : public CXTPPropertyGridItem
{
public:
	CCustomItemUpperCase::CCustomItemUpperCase(CString strCaption)
		: CXTPPropertyGridItem(strCaption)
	{
	}

protected:
	virtual CXTPPropertyGridInplaceEdit& GetInplaceEdit()
	{
		return m_wndEdit;
	}

private:
	CInplaceUpperCase m_wndEdit;

};


class CCustomItemIPAddress : public CXTPPropertyGridItem
{
	class CInplaceEditIPAddress : public CXTPPropertyGridInplaceEdit
	{
	public:
		BOOL Create(LPCTSTR /*lpszClassName*/, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
		{		
			if (!CWnd::Create(WC_IPADDRESS, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
				return FALSE;
			
			ModifyStyle(WS_BORDER, 0);
			ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);		
			return TRUE;
		}
	};

public:
	CCustomItemIPAddress::CCustomItemIPAddress(CString strCaption)
		: CXTPPropertyGridItem(strCaption)
	{
	}

protected:
	virtual CXTPPropertyGridInplaceEdit& GetInplaceEdit()
	{
		return m_wndEdit;
	}

private:
	CInplaceEditIPAddress m_wndEdit;

};


class CCustomItemMenu : public CXTPPropertyGridItem
{
public:
	CCustomItemMenu::CCustomItemMenu(CString strCaption)
		: CXTPPropertyGridItem(strCaption)
	{
		SetFlags(xtpGridItemHasEdit | xtpGridItemHasComboButton);
	}

	virtual void OnInplaceButtonDown(CXTPPropertyGridInplaceButton* pButton);
};




class CCustomItemSlider;

class CInplaceSlider : public CSliderCtrl
{
public:
	CInplaceSlider()
	{
		m_enable = true;
	}
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT /*nCtlColor*/);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void UpdateCursorPos(CPoint point);
	void DrawSliderBar(CDC* pDC);
	afx_msg BOOL OnEraseBkgnd(CDC* /*pDC*/);

	float GetValue() {return m_value;}
	void SetValue(float value);
	void SetRange(float min, float max);

	void SetHighlight(bool enable);

	float m_floatScale;
protected:
	CCustomItemSlider* m_pItem;
	COLORREF m_clrFore;
	COLORREF m_clrGrey;
	COLORREF m_clrBack;
	CBrush m_brBack;

	float m_value;
	float m_min;
	float m_max;

	CPoint m_ptMousePos;

	bool m_enable;

	friend class CCustomItemSlider;
};

class CCustomItemSlider : public CXTPPropertyGridItemDouble
{
protected:

public:
	CCustomItemSlider(CString strCaption, float fMin, float fMax);

	virtual void SetDouble( double fValue );
	void BindToFloat(float* floatVal)
	{
		m_pBindFloat = floatVal;
	}

protected:
	virtual void OnDeselect();
	virtual void OnSelect();

	virtual void OnBeforeInsert();



	//virtual void OnDrawItemConstraint(CDC* pDC, CXTPPropertyGridItemConstraint* pConstraint, CRect rc, BOOL bSelected);

private:
	CInplaceSlider m_wndSlider;
	int m_nWidth;
	float m_floatScale;
	float* m_pBindFloat;

	friend class CInplaceSlider;
};


class CCustomItemButton;

class CInplaceButton : public CXTButton
{
public:
	DECLARE_MESSAGE_MAP()
protected:
	CCustomItemButton* m_pItem;
	void OnClicked();
	BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	friend class CCustomItemButton;
};

class CCustomItemButton : public CXTPPropertyGridItem
{
protected:

public:
	CCustomItemButton(CString strCaption, BOOL bFullRowButton, BOOL bValue);

	BOOL GetBool();
	void SetBool(BOOL bValue);

protected:
	virtual BOOL IsValueChanged();
	virtual void SetVisible(BOOL bVisible);
	BOOL OnDrawItemValue(CDC& dc, CRect rcValue);
	virtual void OnIndexChanged();
	void CreateButton();


private:
	CInplaceButton m_wndButton;
	BOOL m_bValue;
	CFont m_wndFont;
	CString m_strButtonText;
	BOOL m_bFullRowButton;

	friend class CInplaceButton;
};


