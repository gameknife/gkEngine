// gkDoc.cpp : implementation of the gkDoc class
//

#include "stdafx.h"
#include "RibbonSample.h"

#include "RibbonSampleDoc.h"
#include "gkFilePath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// gkDoc

IMPLEMENT_DYNCREATE(CRibbonSampleDoc, CDocument)

BEGIN_MESSAGE_MAP(CRibbonSampleDoc, CDocument)
END_MESSAGE_MAP()


// gkDoc construction/destruction

CRibbonSampleDoc::CRibbonSampleDoc()
{
	// TODO: add one-time construction code here

}

CRibbonSampleDoc::~CRibbonSampleDoc()
{
}

BOOL CRibbonSampleDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}
//-----------------------------------------------------------------------
BOOL CRibbonSampleDoc::OnOpenDocument( LPCTSTR lpszPathName )
{
	gkStdString levelpath = gkGetGameRelativePath(lpszPathName);

	GetIEditor()->loadScene(levelpath.c_str());
	return TRUE;
}

BOOL CRibbonSampleDoc::OnSaveDocument( LPCTSTR lpszPathName )
{
	GetIEditor()->saveScene(lpszPathName);	
	return true;
}



// gkDoc serialization

void CRibbonSampleDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here

	}
	else
	{
		// TODO: add loading code here
	}
}


// gkDoc diagnostics

#ifdef _DEBUG
void CRibbonSampleDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRibbonSampleDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}



#endif //_DEBUG


// gkDoc commands
