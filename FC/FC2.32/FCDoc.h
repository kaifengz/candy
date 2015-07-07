// FCDoc.h : interface of the CFCDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FCDOC_H__3B3A0B62_061C_41EE_9433_F7C38A50AC75__INCLUDED_)
#define AFX_FCDOC_H__3B3A0B62_061C_41EE_9433_F7C38A50AC75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CFCDoc : public CDocument
{
protected: // create from serialization only
	CFCDoc();
	DECLARE_DYNCREATE(CFCDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFCDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFCDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFCDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FCDOC_H__3B3A0B62_061C_41EE_9433_F7C38A50AC75__INCLUDED_)
