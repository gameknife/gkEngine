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
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	gkMaterialEditor.h
// Desc:		
// 
// Author:  Kaiming
// Date:	2011/11/3 
// Modify:	2011/11/3
// 
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "..\Resource.h"
#include "gkToolBarPaneBase.h"
#include "Panels/gkPaneBase.h"
#include "Controls\CustomSplitter.h"
#include "Controls\gkPropertyGirdEx.h"
// gkMaterialEditor dialog

#define MATEDT_PARAMSTART 10u
#define MATEDT_PARAMSSTAT 500u
#define MATEDT_MACROSET 555u

#define MATEDT_SHADERSET 655u

#define MATEDT_TEXSET 750u

class gkMaterialPtr;
class IMaterial;

struct SMatItemInfo
{
	HTREEITEM m_hItem;
	IMaterial* m_pMaterial;

	SMatItemInfo()
	{
		m_hItem = 0;
		m_pMaterial = 0;
	}
};

class gkMaterialEditor : public gkToolBarPaneBase, public IPSCPColorChangeCallback
{
	//DECLARE_DYNAMIC(gkMaterialEditor)

public:
	gkMaterialEditor(CWnd* pParent = NULL);   // standard constructor
	virtual ~gkMaterialEditor();

// Dialog Data
	enum { IDD = IDD_MATERIALEDITOR };

	CStatic m_wndPlaceHolder;
	gkPropertyGirdEx m_wndPropertyGrid;
	CXTTreeCtrl	m_treeMaterialList;
	CImageList m_imageList;

	IMaterial* m_curMtl;
	CString m_curSelMtlName;
	int m_curSelMtlParaNum;

	CSplitterWndEx m_wndHSplitter;
	CSplitterWndEx m_wndVSplitter;

	typedef std::map<CString, HTREEITEM> NameTreeMap;
	NameTreeMap m_namemap;

	typedef std::vector<SMatItemInfo*> MatItemSet;
	MatItemSet m_items;

	BOOL InitPropertyTable();
	void RefreshMaterialList();

	void AddMaterialIntoTree(IMaterial* mat);
	void InsertMaterialIntoTree(IMaterial* mat, HTREEITEM parent, int32 nMatSlot = -1);

	void RefreshMtl( IMaterial* curMtl );


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	void OnEmptyCommandRange(UINT);

	DECLARE_MESSAGE_MAP()

public:
	LRESULT OnGridNotify(WPARAM wParam, LPARAM lParam);
	void ModifyCurMaterialDword(int index, uint32 value);
	void ModifyCurMaterialTexture(int index, const TCHAR* value);

	afx_msg void OnBnClickedMaterialGet();
	afx_msg void OnBnClickedMaterialSet();
	afx_msg void OnBnClickedMatedtrefresh();
	afx_msg void OnBnClickedMaterialSaveAll();

public:
	virtual const TCHAR* getPanelName() {return _T("Material Editor");}
	virtual UINT getIID() {return IDD;}
	
	afx_msg void OnTreeBrowserClick(NMHDR *pNMHDR, LRESULT *pResult);

	virtual void OnColorChange( COLORREF clr );

};
