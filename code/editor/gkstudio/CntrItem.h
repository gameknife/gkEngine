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



// CntrItem.h : interface of the CRibbonSampleCntrItem class
//

#if !defined(AFX_CNTRITEM_H__4AB69C50_A727_4E11_ABA5_885476ECD247__INCLUDED_)
#define AFX_CNTRITEM_H__4AB69C50_A727_4E11_ABA5_885476ECD247__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRibbonSampleDoc;
class CRibbonSampleView;

class CRibbonSampleCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CRibbonSampleCntrItem)

// Constructors
public:
	CRibbonSampleCntrItem(REOBJECT* preo = NULL, CRibbonSampleDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	CRibbonSampleDoc* GetDocument()
		{ return (CRibbonSampleDoc*)CRichEditCntrItem::GetDocument(); }
	CRibbonSampleView* GetActiveView()
		{ return (CRibbonSampleView*)CRichEditCntrItem::GetActiveView(); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRibbonSampleCntrItem)
	public:
		int m_nImage;
	protected:
	//}}AFX_VIRTUAL

		
	void OnDeactivateUI(BOOL bUndoable);
	void OnActivate();
	void ActiveStateChanged(BOOL bActive);


// Implementation
public:
	~CRibbonSampleCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CNTRITEM_H__4AB69C50_A727_4E11_ABA5_885476ECD247__INCLUDED_)
