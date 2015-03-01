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
// Name:   	SwissKnifePaneCreation.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/5/19
// Modify:	2012/5/19
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _SwissKnifePaneCreation_h_
#define _SwissKnifePaneCreation_h_

struct SFileItemInfo
{
	HTREEITEM m_hItem;
	gkStdString m_wstrFullname;
	int type;

	SFileItemInfo()
	{
		m_hItem = 0;
		m_wstrFullname = _T("");
		type = 0;
	}
};

class CSwissKnifePaneCreation : public CXTPShortcutBarPane
{
public:
	CSwissKnifePaneCreation();
	virtual ~CSwissKnifePaneCreation();

	BOOL Create(LPCTSTR lpszCaption, CXTPShortcutBar* pParent);

	CXTPTreeCtrl m_wndTreeCategories;
	CXTPTreeCtrl m_wndTreeDetail;
	CImageList m_imageList;

	void UpdateTheme(BOOL bOffice2010Theme);

	void CreateCategories();
	void CreateStaticGeoDetail();

	void ClearDetailTree();

	void CreateGamoObjectDetail();
	void CreateLightDetail();
	void CreateCameraDetail();

private:
	TCHAR m_wstrNextName[MAX_PATH];
	const TCHAR* findNextName(const gkStdString& prefix);

	void AddFileItemIntoTree( CString fullname, int type );
	void InsertFileItemIntoTree( CString fullname, HTREEITEM parent, int type );

	typedef std::map<CString, HTREEITEM> NameTreeMap;
	NameTreeMap m_namemap;

	typedef std::vector<SFileItemInfo*> FileItemSet;
	FileItemSet m_items;

protected:

	
	//}}AFX_MSG

	afx_msg void OnCatePaneClick( NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDetailPaneClick( NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()

};

#endif
