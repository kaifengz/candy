// ScoreDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RJ.h"
#include "ScoreDlg.h"

#include "defines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScoreDlg dialog


CScoreDlg::CScoreDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScoreDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScoreDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CScoreDlg::CScoreDlg(int *pScoreTotal, int *pScoreCur, CWnd *pParent /*=NULL*/)
	: CDialog(CScoreDlg::IDD, pParent)
{
	m_pScoreTotal = pScoreTotal;
	m_pScoreCur = pScoreCur;
}

void CScoreDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScoreDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScoreDlg, CDialog)
	//{{AFX_MSG_MAP(CScoreDlg)
	ON_BN_CLICKED(IDC_RESCORE, OnRescore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScoreDlg message handlers

BOOL CScoreDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetDlgItemInt(IDC_STATIC_EAST_T,	m_pScoreTotal[ID_EAST],		TRUE);
	SetDlgItemInt(IDC_STATIC_NORTH_T,	m_pScoreTotal[ID_NORTH],	TRUE);
	SetDlgItemInt(IDC_STATIC_WEST_T,	m_pScoreTotal[ID_WEST],		TRUE);
	SetDlgItemInt(IDC_STATIC_SOUTH_T,	m_pScoreTotal[ID_SOUTH],	TRUE);
	SetDlgItemInt(IDC_STATIC_EAST_C,	m_pScoreCur[ID_EAST],	TRUE);
	SetDlgItemInt(IDC_STATIC_NORTH_C,	m_pScoreCur[ID_NORTH],	TRUE);
	SetDlgItemInt(IDC_STATIC_WEST_C,	m_pScoreCur[ID_WEST],	TRUE);
	SetDlgItemInt(IDC_STATIC_SOUTH_C,	m_pScoreCur[ID_SOUTH],	TRUE);

	CString sRankInfo;
	sRankInfo.Format(IDS_RANK_INFO, GetRankString(m_pScoreTotal));
	SetDlgItemText(IDC_STATIC_RANK_INFO, sRankInfo);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return TRUE
}

void CScoreDlg::OnRescore() 
{
	for(int i=0; i<PLAYER_NUM; i++)
	{
		m_pScoreTotal[i] = 0;
		m_pScoreCur[i] = 0;
	}

	SetDlgItemInt(IDC_STATIC_EAST_T,	m_pScoreTotal[ID_EAST],		TRUE);
	SetDlgItemInt(IDC_STATIC_NORTH_T,	m_pScoreTotal[ID_NORTH],	TRUE);
	SetDlgItemInt(IDC_STATIC_WEST_T,	m_pScoreTotal[ID_WEST],		TRUE);
	SetDlgItemInt(IDC_STATIC_SOUTH_T,	m_pScoreTotal[ID_SOUTH],	TRUE);
	SetDlgItemInt(IDC_STATIC_EAST_C,	m_pScoreCur[ID_EAST],	TRUE);
	SetDlgItemInt(IDC_STATIC_NORTH_C,	m_pScoreCur[ID_NORTH],	TRUE);
	SetDlgItemInt(IDC_STATIC_WEST_C,	m_pScoreCur[ID_WEST],	TRUE);
	SetDlgItemInt(IDC_STATIC_SOUTH_C,	m_pScoreCur[ID_SOUTH],	TRUE);
}

CString CScoreDlg::GetRankString(const int nScoreTotal[])
{
	int nRank=0, nSame=0;
	UINT nRankStringID;
	CString sRankString;

	for(int i=0; i<PLAYER_NUM; i++)
	{
		if(nScoreTotal[i] > nScoreTotal[ID_PLAYER])
			nRank++;
		if(nScoreTotal[i] == nScoreTotal[ID_PLAYER])
			nSame++;
	}

	switch(nRank)
	{
	case 0:	nRankStringID = IDS_RANK_1;	break;
	case 1:	nRankStringID = IDS_RANK_2;	break;
	case 2:	nRankStringID = IDS_RANK_3;	break;
	case 3:	nRankStringID = IDS_RANK_4;	break;
	}
	if(nRank+nSame == PLAYER_NUM) // 分数最低	
	{
		nRankStringID = IDS_RANK_4;
	}
	if(nSame == PLAYER_NUM) // 分数都一样
	{
		nRankStringID = IDS_RANK_ALL_THE_SAME;
	}

	sRankString.LoadString(nRankStringID);
	return sRankString;
}
