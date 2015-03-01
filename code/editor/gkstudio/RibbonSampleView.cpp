// gkView.cpp : implementation of the gkView class
//

#include "stdafx.h"
#include "RibbonSample.h"

#include "RibbonSampleDoc.h"
#include "RibbonSampleView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// gkView

IMPLEMENT_DYNCREATE(CRibbonSampleView, CView)

BEGIN_MESSAGE_MAP(CRibbonSampleView, CView)
END_MESSAGE_MAP()

// gkView construction/destruction

CRibbonSampleView::CRibbonSampleView()
{
	// TODO: add construction code here

}

CRibbonSampleView::~CRibbonSampleView()
{
}

BOOL CRibbonSampleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CView::PreCreateWindow(cs);
}

// gkView drawing

void CRibbonSampleView::OnDraw(CDC* /*pDC*/)
{
	CRibbonSampleDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// gkView diagnostics

#ifdef _DEBUG
void CRibbonSampleView::AssertValid() const
{
	CView::AssertValid();
}

void CRibbonSampleView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRibbonSampleDoc* CRibbonSampleView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRibbonSampleDoc)));
	return (CRibbonSampleDoc*)m_pDocument;
}
//-----------------------------------------------------------------------
// int CRibbonSampleView::OnCreateContent()
// {	
// 	
// 
// 	return 0;
// }

#endif //_DEBUG


// gkView message handlers
