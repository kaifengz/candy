// FCDoc.cpp : implementation of the CFCDoc class
//

#include "stdafx.h"
#include "FC.h"

#include "FCDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFCDoc

IMPLEMENT_DYNCREATE(CFCDoc, CDocument)

BEGIN_MESSAGE_MAP(CFCDoc, CDocument)
	//{{AFX_MSG_MAP(CFCDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFCDoc construction/destruction

CFCDoc::CFCDoc()
{
	// TODO: add one-time construction code here

}

CFCDoc::~CFCDoc()
{
}

BOOL CFCDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	((CEditView*)m_viewList.GetHead())->SetWindowText(NULL);

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CFCDoc serialization

void CFCDoc::Serialize(CArchive& ar)
{
	// CEditView contains an edit control which handles all serialization
	((CEditView*)m_viewList.GetHead())->SerializeRaw(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CFCDoc diagnostics

#ifdef _DEBUG
void CFCDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFCDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFCDoc commands
