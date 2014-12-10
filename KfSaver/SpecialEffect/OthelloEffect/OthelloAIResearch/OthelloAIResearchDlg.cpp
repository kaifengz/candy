
// OthelloAIResearchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OthelloAIResearch.h"
#include "OthelloAIResearchDlg.h"
#include "afxdialogex.h"

#include "EstimatorRegistry.h"
#include "OthelloSearchNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COthelloAIResearchDlg dialog




COthelloAIResearchDlg::COthelloAIResearchDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COthelloAIResearchDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COthelloAIResearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BLACK, m_black);
	DDX_Control(pDX, IDC_WHITE, m_white);
	DDX_Control(pDX, IDC_ROUNDS, m_rounds);
	DDX_Control(pDX, IDC_START, m_start);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_MAJOR, m_major);
	DDX_Control(pDX, IDC_MINOR, m_minor);
}

BEGIN_MESSAGE_MAP(COthelloAIResearchDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &COthelloAIResearchDlg::OnBnClickedStart)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// COthelloAIResearchDlg message handlers

BOOL COthelloAIResearchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	vector<string> estimators;
	EstimatorRegistry::GetInstance()->GetAllEstimators(estimators);
	for (unsigned i=0; i<estimators.size(); ++i)
	{
		m_black.AddString(estimators[i].c_str());
		m_white.AddString(estimators[i].c_str());
	}
	if (!estimators.empty())
	{
		m_black.SetCurSel(0);
		m_white.SetCurSel(0);
	}

	m_rounds.SetWindowText("10");
	m_board.SetInitState();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void COthelloAIResearchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);

		const int offset = 8;
		const int size = 36;

		dc.SelectStockObject(BLACK_PEN);
		for (int i=0; i<9; ++i)
		{
			dc.MoveTo(offset + 0*size, offset + i*size);
			dc.LineTo(offset + 8*size, offset + i*size);
			dc.MoveTo(offset + i*size, offset + 0*size);
			dc.LineTo(offset + i*size, offset + 8*size);
		}

		dc.SelectStockObject(BLACK_PEN);
		dc.SelectStockObject(WHITE_BRUSH);
		for (int i=0; i<8; ++i)
		{
			for (int j=0; j<8; ++j)
			{
				if (m_board.Get(i, j) != COthelloBoard::WHITE)
					continue;
				int x = offset + i*size;
				int y = offset + j*size;
				dc.Ellipse(x + 2, y + 2, x + size - 2, y + size - 2);
			}
		}

		dc.SelectStockObject(BLACK_PEN);
		dc.SelectStockObject(BLACK_BRUSH);
		for (int i=0; i<8; ++i)
		{
			for (int j=0; j<8; ++j)
			{
				if (m_board.Get(i, j) != COthelloBoard::BLACK)
					continue;
				int x = offset + i*size;
				int y = offset + j*size;
				dc.Ellipse(x + 2, y + 2, x + size - 2, y + size - 2);
			}
		}
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR COthelloAIResearchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void COthelloAIResearchDlg::OnBnClickedStart()
{
	m_board.SetInitState();
	m_curr_rounds = 0;
	m_black_major = 0;
	m_white_major = 0;
	m_black_minor = 0;
	m_white_minor = 0;

	CString rounds;
	m_rounds.GetWindowText(rounds);
	m_total_rounds = atoi(rounds);

	Invalidate(TRUE);

	m_black.EnableWindow(FALSE);
	m_white.EnableWindow(FALSE);
	m_rounds.EnableWindow(FALSE);
	m_start.EnableWindow(FALSE);
	FormatStatistic();

	SetTimer(17333, 50, NULL);
}

void COthelloAIResearchDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialogEx::OnTimer(nIDEvent);
	if (nIDEvent != 17333)
		return;

	const int depth = 6;

	vector<string> estimators;
	EstimatorRegistry *registry = EstimatorRegistry::GetInstance();
	registry->GetAllEstimators(estimators);

	string black_name = estimators[m_black.GetCurSel()];
	string white_name = estimators[m_white.GetCurSel()];
	auto_ptr<IEstimator> black(registry->CreateEstimator(black_name.c_str()));
	auto_ptr<IEstimator> white(registry->CreateEstimator(white_name.c_str()));

	IEstimator *first=0, *second=0;

	if (m_board.GetNext() == COthelloBoard::BLACK)
	{
		first = black.get();
		second = white.get();
	}
	else
	{
		first = white.get();
		second = black.get();
	}

	Movement move;
	COthelloSearchNode search(m_board, *first);
	if (search.GetBestMovement(move, depth))
		m_board.Set(move.x, move.y, move.cell);
	else
	{
		m_board.SkipOne();

		COthelloSearchNode search(m_board, *second);
		if (search.GetBestMovement(move, depth))
			m_board.Set(move.x, move.y, move.cell);
		else
		{
			int blacks=0, whites=0;
			for (int i=0; i<8; ++i)
			{
				for (int j=0; j<8; ++j)
				{
					switch (m_board.Get(i, j))
					{
					case COthelloBoard::BLACK: ++blacks; break;
					case COthelloBoard::WHITE: ++whites; break;
					}
				}
			}

			if (blacks > whites)
				++m_black_major;
			else if (whites > blacks)
				++m_white_major;
			m_black_minor += blacks;
			m_white_minor += whites;

			if (++m_curr_rounds < m_total_rounds)
			{
				m_board.SetInitState();
				Invalidate(TRUE);
			}
			else
			{
				KillTimer(17333);

				m_black.EnableWindow(TRUE);
				m_white.EnableWindow(TRUE);
				m_rounds.EnableWindow(TRUE);
				m_start.EnableWindow(TRUE);
			}

			FormatStatistic();
		}
	}

	Invalidate(FALSE);
}

void COthelloAIResearchDlg::FormatStatistic()
{
	CString tmp;

	tmp.Format("%d / %d", m_curr_rounds, m_total_rounds);
	m_progress.SetWindowText(tmp);

	tmp.Format("%d : %d", m_black_major, m_white_major);
	m_major.SetWindowTextA(tmp);

	tmp.Format("%d : %d", m_black_minor, m_white_minor);
	m_minor.SetWindowTextA(tmp);
}
