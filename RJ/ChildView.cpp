// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "RJ.h"
#include "ChildView.h"
#include "MainFrm.h"

#include <algorithm>
using std::sort;

#include "ScoreDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef _DEBUG
#	ifdef ASSERT
#		undef ASSERT
#	endif

#	define ASSERT(f)	if(f);else ErrorReport(__FILE__, __LINE__)
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
	// attributes
	m_gs = GS_PRESTART;
	m_pBmpClient = NULL;
	m_iPopup = -1;
	m_iSpecialShow = -1;

	// options
	m_bAnimal = TRUE;
	m_bUserAuto = FALSE;
	m_bShowComputerCard = FALSE;
	m_nThinkingTimeslice = TIMESLICE_MID;
	m_ai = AI_NORMAL;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_COMMAND(ID_NEW_GAME, OnNewGame)
	ON_WM_TIMER()
	ON_COMMAND(ID_SHOW_COMPUTER_CARD, OnShowComputerCard)
	ON_COMMAND(ID_PLAYER_AUTO, OnPlayerAuto)
	ON_UPDATE_COMMAND_UI(ID_PLAYER_AUTO, OnUpdatePlayerAuto)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_SCORE, OnScore)
	ON_COMMAND(ID_ANIMAL, OnAnimal)
	ON_UPDATE_COMMAND_UI(ID_ANIMAL, OnUpdateAnimal)
	ON_COMMAND(ID_HINT, OnHint)
	ON_COMMAND(ID_SEND, OnSend)
	ON_UPDATE_COMMAND_UI(ID_SEND, OnUpdateSend)
	ON_UPDATE_COMMAND_UI(ID_HINT, OnUpdateHint)
	ON_COMMAND(ID_TERMINATE_GAME, OnTerminateGame)
	ON_UPDATE_COMMAND_UI(ID_TERMINATE_GAME, OnUpdateTerminateGame)
	ON_COMMAND_RANGE(ID_SPEED_LOW, ID_SPEED_HIGH, OnSpeed)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SPEED_LOW, ID_SPEED_HIGH, OnUpdateSpeed)
	ON_COMMAND(ID_SPEED_UP, OnSpeedUp)
	ON_COMMAND(ID_SPEED_DOWN, OnSpeedDown)
	ON_COMMAND_RANGE(ID_DEGREE_EASY, ID_DEGREE_HARD, OnAI)
	ON_UPDATE_COMMAND_UI_RANGE(ID_DEGREE_EASY, ID_DEGREE_HARD, OnUpdateAI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;

	cs.lpszClass = AfxRegisterWndClass(
		CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW),
		HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//	drawing
///////////////////////////////////////////////////////////////////////////////

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if(m_pBmpClient == NULL)
	{
		InitProgram();

		m_pBmpClient = new CBitmap();
		m_pBmpClient->CreateCompatibleBitmap(
			&dc,
			m_rcClient.Width(),
			m_rcClient.Height());

		m_bRedraw = TRUE;
	}

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	dcMem.SelectObject(m_pBmpClient);

	if(m_bRedraw)
	{
		m_bRedraw = FALSE;
		DrawAll(&dcMem);
	}

	dc.BitBlt(0, 0, m_rcClient.Width(), m_rcClient.Height(), &dcMem, 0, 0, SRCCOPY);
}

void CChildView::DrawAll(CDC *pDC)
{
	pDC->SelectStockObject(NULL_PEN);

	{	// back ground
		CBrush brushBG(&m_bmpBackground);
		pDC->SelectObject(&brushBG);
		pDC->Rectangle(0, 0, m_rcClient.Width(), m_rcClient.Height());
	}

	{	// player cards
		ZPlayerCards::iterator iter;
		ZPlayerCards::iterator begin, end;
		BOOL bShowCard;

		for(int i=0; i<PLAYER_NUM; i++)
		{
			begin = m_PlayerCards[i].begin();
			if(m_gs == GS_DISPATCH)
				end = m_PlayerCards[i].begin()+m_iAnimalCount;
			else
				end = m_PlayerCards[i].end();

			for(iter=begin; iter!=end; iter++)
			{
				if(i!=ID_PLAYER && !m_bShowComputerCard)
				{
					bShowCard = FALSE;
				}
				else if(m_gs==GS_CLEAN_UP)
				{
					bShowCard = (m_iAnimalCount==0) ? FALSE : TRUE;
				}
				else
				{
					bShowCard = TRUE;
				}

				DrawCard(
					pDC,
					bShowCard ? (&m_bmpCard[iter->iCard]) : (&m_bmpCardBack),
					iter->pos.x,
					iter->pos.y);
			}
		}

		// special show card
		if(m_iSpecialShow >= 0)
		{
			const ZCard &card = m_PlayerCards[ID_PLAYER][m_iSpecialShow];

			DrawCard(
				pDC,
				&m_bmpCard[card.iCard],
				card.pos.x,
				card.pos.y);
		}
	}

	{	// card lists
		for(int i=0; i<LIST_NUM; i++)
		{
			if(m_CardList[i].first<0)
				continue;
			for(int j=m_CardList[i].first; j<=m_CardList[i].last; j++)
			{
				DrawCard(
					pDC,
					&m_bmpCard[j],
					LEFT_LIST(m_rcClient.Width()) + LIST_INTERVAL_H*(LIST_NUM-1-i),
					TOP_LIST(m_rcClient.Height()) + LIST_INTERVAL_V*(j%LIST_EACH) );
			}
		}
	}

	// moving(sending or transfering) card
	// 当正在传送牌的时候，如果用户突然关闭动画功能，m_bAnimal即变成FALSE，但此时
	// 界面上应该把牌传送完毕，所以此处有此改动
//	if(m_bAnimal && (m_gs==GS_SENDING_CARD || m_gs==GS_TRANSFER_CARD))
	if(m_gs==GS_SENDING_CARD || m_gs==GS_TRANSFER_CARD)
	{
		// 出牌的时候显示牌
		// 传牌的时候只有在玩家或下家拿牌时才显示牌（若显示电脑的牌则一直显示牌）
		BOOL bShowCard = FALSE;

		if(m_gs==GS_SENDING_CARD)
		{
			bShowCard = TRUE;
		}
		else if(m_iCurrentTurn == (ID_PLAYER+1)%PLAYER_NUM ||
				m_iCurrentTurn == ID_PLAYER ||
				m_bShowComputerCard)
		{
			bShowCard = TRUE;
		}

		DrawCard(
			pDC,
			bShowCard ? (&m_bmpCard[m_iMovingCard]) : (&m_bmpCardBack),
			m_ptMovingCard.x,
			m_ptMovingCard.y);
	}

	// token
	switch(m_gs)
	{
	case GS_PRESTART:
	case GS_DISPATCH:
	case GS_CLEAN_UP:
	case GS_END:
		break;

	default:
		DrawToken(pDC);
		break;
	}
}

void CChildView::DrawCard(CDC *pDC, CBitmap *pBmpCard, int x, int y)
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	dcMem.SelectObject(&m_bmpCardShape);
	pDC->BitBlt(x, y, CARD_WIDTH, CARD_HEIGHT, &dcMem, 0, 0, SRCAND);

	dcMem.SelectObject(pBmpCard);
	pDC->BitBlt(x, y, CARD_WIDTH, CARD_HEIGHT, &dcMem, 0, 0, SRCPAINT);
}

void CChildView::DrawToken(CDC *pDC)
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	dcMem.SelectObject(&m_bmpTokenShape);
	pDC->BitBlt(m_ptToken.x, m_ptToken.y, TOKEN_WIDTH, TOKEN_HEIGHT,
		&dcMem, 0, 0, SRCAND);

	dcMem.SelectObject(&m_bmpToken);
	pDC->BitBlt(m_ptToken.x, m_ptToken.y, TOKEN_WIDTH, TOKEN_HEIGHT,
		&dcMem, 0, 0, SRCPAINT);
}

void CChildView::FitCardBitmap(CBitmap &bitmap)
{
	CDC dc;
	dc.CreateCompatibleDC(NULL);
	dc.SelectObject(&bitmap);

	CDC dc2;
	dc2.CreateCompatibleDC(&dc);
	dc2.SelectObject(&m_bmpCardShape);

	dc.BitBlt(0, 0, CARD_WIDTH, CARD_HEIGHT, &dc2, 0, 0, DSTINVERT);	//dest = (NOT dest)
	dc.BitBlt(0, 0, CARD_WIDTH, CARD_HEIGHT, &dc2, 0, 0, NOTSRCERASE);	//dest = (NOT src) AND (NOT dest)
}

///////////////////////////////////////////////////////////////////////////////
//	游戏流程控制函数
///////////////////////////////////////////////////////////////////////////////

void CChildView::NewGame() 
{
	m_iCurrentTurn = -1;
	m_iMovingCard = -1;
	m_iPopup = -1;
	m_iSpecialShow = -1;

	// 清除原来可能的定时器
	KillTimer(IDT_DISPATCH);
	KillTimer(IDT_CLEAN_UP);
	KillTimer(IDT_THINKING);
	KillTimer(IDT_CARD_SENDING);
	KillTimer(IDT_TOKEN_TRANSFER);
	KillTimer(IDT_CARD_SELECT);
	KillTimer(IDT_CARD_TRANSFER);

	// 初始化随机数
	srand(GetTickCount());

	// 洗牌并计算牌的位置
	Shuffle();
	CalcCardPos(FALSE);

	// 清空已经打出的牌
	for(int i=0; i<LIST_NUM; i++)
	{
		m_CardList[i].first = -1;
		m_CardList[i].last = -1;
	}

	// 更新状态栏提示
	UpdateRemainInfo();

	// 启动下一部动作
	if(m_bAnimal)
	{
		m_gs = GS_DISPATCH;
		m_iAnimalCount = 0;
		SetTimer(IDT_DISPATCH, TIMESLICE_DISPATCH, NULL);
		SetStatusBarInfo(IDS_DISPATCHING);
	}
	else
	{
		DispatchOver();
	}

	m_bRedraw = TRUE;
	Invalidate(FALSE);
}

void CChildView::DispatchOver()
{
	if(m_bAnimal)
	{
		m_gs = GS_CLEAN_UP;
		m_iAnimalCount = 0;
		SetTimer(IDT_CLEAN_UP, TIMESLICE_CLEAN_UP, NULL);
	}
	else
	{
		CleanUpCards();
		CleanUpOver();
	}

	m_bRedraw = TRUE;
	Invalidate(FALSE);
}

void CChildView::CleanUpOver()
{
	CalcTokenStaticPos();
	m_iCurrentTurn = FindFirstOut();

	if(m_iCurrentTurn==ID_PLAYER && !m_bUserAuto)
	{
		m_gs = GS_THINKING;
		CalcTokenPos();

		// 等待玩家鼠标输入
		SetWaitSendInfo();
	}
	else
	{
		// 电脑先出牌，启动思考定时器
		m_gs = GS_THINKING;
		CalcTokenPos();

		SetTimer(IDT_THINKING, m_nThinkingTimeslice, NULL);
		SetWaitSendInfo();
	}

	m_bRedraw = TRUE;
	Invalidate(FALSE);
}

void CChildView::ThinkingOver()
{
	if(m_bAnimal)
	{
		m_gs = GS_SENDING_CARD;
		m_iAnimalCount = 0;
		SetTimer(IDT_CARD_SENDING, TIMESLICE_CARD_SENDING, NULL);
		SetSendInfo();
	}
	else
	{
		CardSendingOver();
	}

	m_bRedraw = TRUE;
	Invalidate(FALSE);
}

void CChildView::CardSendingOver()
{
	AcceptSendingCard();
	CalcCardPos(TRUE);

	UpdateRemainInfo();

	if(IsGameOver())
	{
		m_gs = GS_END;
		ProcessGameOver();
	}
	else
	{
		if(m_bAnimal)
		{
			m_gs = GS_TRANSFER_TOKEN;
			m_iAnimalCount = 0;
			SetTimer(IDT_TOKEN_TRANSFER, TIMESLICE_TOKEN_TRANSFER, NULL);
			SetCurrentTurnInfo();
		}
		else
		{
			TokenTransferOver();
		}

		m_bRedraw = TRUE;
		Invalidate(FALSE);
	}
}

void CChildView::TokenTransferOver()
{
	CalcTokenStaticPos();
	m_iCurrentTurn = (m_iCurrentTurn+1)%PLAYER_NUM;

	if(CardCanOut() > 0) // 有牌可以打出
	{
		m_gs = GS_THINKING;
		CalcTokenPos();

		if(m_iCurrentTurn==ID_PLAYER && !m_bUserAuto)
		{
			// 等待玩家鼠标输入
			SetWaitSendInfo();
		}
		else
		{
			SetTimer(IDT_THINKING, m_nThinkingTimeslice, NULL);
			SetWaitSendInfo();
		}
	}
	else // 没有牌可以打出
	{
		m_gs = GS_SELECT_CARD;
		CalcTokenPos();

		int dir = (m_iCurrentTurn+PLAYER_NUM-1)%PLAYER_NUM; // 上家

		// 给上家洗牌，以便下家从中抽取一张牌
		// must call after CalcTokenPos
		ShufflePlayerCards(dir);
		CalcCardPos(dir, FALSE);

		if(m_iCurrentTurn==ID_PLAYER && !m_bUserAuto)
		{
			// 等待玩家鼠标输入
			SetSelectInfo();
		}
		else
		{
			m_iSelectCardIndex = rand()%m_PlayerCards[dir].size();

			SetTimer(IDT_CARD_SELECT, TIMESLICE_CARD_SELECT, NULL);
			SetSelectInfo();
		}
	}

	m_bRedraw = TRUE;
	Invalidate(FALSE);
}

void CChildView::CardSelectOver()
{
	int dir = (m_iCurrentTurn+PLAYER_NUM-1)%PLAYER_NUM;	// 上家

	// 计算该牌的移动
	m_iMovingCard = m_PlayerCards[dir][m_iSelectCardIndex].iCard;
	m_ptMovingCard = m_PlayerCards[dir][m_iSelectCardIndex].pos;
	m_ptMovingCardSource = m_ptMovingCard;
	m_ptMovingCardDest = m_PlayerCards[m_iCurrentTurn].back().pos;
	switch(m_iCurrentTurn)
	{
	case ID_EAST:
	case ID_WEST:
		m_ptMovingCardDest += CPoint(0, INTERVAL_V);
		break;

	case ID_NORTH:
	case ID_SOUTH:
		m_ptMovingCardDest += CPoint(INTERVAL_H, 0);
		break;

	default:
		ASSERT(0);
		break;
	}

	// 把该牌从上家中去除掉
	m_PlayerCards[dir].erase( m_PlayerCards[dir].begin()+m_iSelectCardIndex );

	if(m_bAnimal)
	{
		m_gs = GS_TRANSFER_CARD;
		m_iAnimalCount = 0;
		SetTimer(IDT_CARD_TRANSFER, TIMESLICE_CARD_TRANSFER, NULL);
		SetSelectOverInfo();
	}
	else
	{
		CardTransferOver();
	}

	m_bRedraw = TRUE;
	Invalidate(FALSE);
}

void CChildView::CardTransferOver()
{
	{	// 把抽取到的牌加入到该Player牌中
		ZCard card;
		card.iCard = m_iMovingCard;
		m_PlayerCards[m_iCurrentTurn].push_back(card);
	}

	CleanUpCards();
	CalcCardPos(TRUE);

	UpdateRemainInfo();

	if(IsGameOver())
	{
		m_gs = GS_END;
		ProcessGameOver();
	}
	else
	{
		if(m_bAnimal)
		{
			m_gs = GS_TRANSFER_TOKEN;
			m_iAnimalCount = 0;
			SetTimer(IDT_TOKEN_TRANSFER, TIMESLICE_TOKEN_TRANSFER, NULL);
			SetCurrentTurnInfo();
		}
		else
		{
			TokenTransferOver();
		}

		m_bRedraw = TRUE;
		Invalidate(FALSE);
	}
}

void CChildView::OnTimer(UINT nIDEvent)
{
	CWnd::OnTimer(nIDEvent);

	KillTimer(nIDEvent);

	switch(nIDEvent)
	{
	case IDT_DISPATCH:
//		ASSERT(m_bAnimal);

		if(++m_iAnimalCount > CARD_EACH_INIT)
		{
			DispatchOver();
		}
		else
		{
			SetTimer(IDT_DISPATCH, TIMESLICE_DISPATCH, NULL);

			m_bRedraw = TRUE;
			Invalidate(FALSE);
		}
		break;

	case IDT_CLEAN_UP:
//		ASSERT(m_bAnimal);

		if(++m_iAnimalCount > 1)
		{
			CleanUpOver();
		}
		else
		{
			CleanUpCards();
			SetTimer(IDT_CLEAN_UP, TIMESLICE_CLEAN_UP, NULL);

			m_bRedraw = TRUE;
			Invalidate(FALSE);
		}
		break;

	case IDT_THINKING:
		SendCard(ComputerThink());
		ThinkingOver();
		break;

	case IDT_CARD_SENDING:
//		ASSERT(m_bAnimal);

		if(++m_iAnimalCount > CARD_SENDING_NUM)
		{
			CardSendingOver();
		}
		else
		{
			CalcMovingCardPos(CARD_SENDING_NUM);
			SetTimer(IDT_CARD_SENDING, TIMESLICE_CARD_SENDING, NULL);

			m_bRedraw = TRUE;
			InvalidateRect(GetCardMovingInvalidRect(CARD_SENDING_NUM), FALSE);
		}

		break;

	case IDT_TOKEN_TRANSFER:
//		ASSERT(m_bAnimal);

		if(++m_iAnimalCount > TOKEN_TRANSFER_NUM)
		{
			TokenTransferOver();
		}
		else
		{
			CalcTokenPos();
			SetTimer(IDT_TOKEN_TRANSFER, TIMESLICE_TOKEN_TRANSFER, NULL);

			m_bRedraw = TRUE;
			InvalidateRect(GetTokenTransferInvalidRect(), FALSE);
		}
		break;

	case IDT_CARD_SELECT:
		// 下家随机的抽取上家一张牌
		m_iSelectCardIndex =
			rand()%m_PlayerCards[(m_iCurrentTurn+PLAYER_NUM-1)%PLAYER_NUM].size();

		CardSelectOver();
		break;

	case IDT_CARD_TRANSFER:
//		ASSERT(m_bAnimal);

		if(++m_iAnimalCount > CARD_TRANSFER_NUM)
		{
			CardTransferOver();
		}
		else
		{
			CalcMovingCardPos(CARD_TRANSFER_NUM);
			SetTimer(IDT_CARD_TRANSFER, TIMESLICE_CARD_TRANSFER, NULL);

			m_bRedraw = TRUE;
			InvalidateRect(GetCardMovingInvalidRect(CARD_TRANSFER_NUM), FALSE);
		}
		break;

	default:
		ASSERT(0);
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
//	Menu Message Handle
///////////////////////////////////////////////////////////////////////////////

void CChildView::OnNewGame()
{
	NewGame();
}

void CChildView::OnPlayerAuto() 
{
	m_bUserAuto = !m_bUserAuto;

	if(m_bUserAuto && m_iCurrentTurn==ID_PLAYER)
	{
		switch(m_gs)
		{
		case GS_THINKING:
			SendCard(ComputerThink());
			ThinkingOver();
			break;

		case GS_SELECT_CARD:
			m_iSelectCardIndex =
				rand()%m_PlayerCards[(m_iCurrentTurn+PLAYER_NUM-1)%PLAYER_NUM].size();

			CardSelectOver();
			break;

		default:
			break;
		}
	}
}

void CChildView::OnUpdatePlayerAuto(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bUserAuto);
}

void CChildView::OnShowComputerCard() 
{
	m_bShowComputerCard = !m_bShowComputerCard;

	m_bRedraw = TRUE;
	Invalidate(FALSE);
}

void CChildView::OnScore() 
{
	BOOL bDispatch		= KillTimer(IDT_DISPATCH);
    BOOL bCleanUp		= KillTimer(IDT_CLEAN_UP);
    BOOL bThinking		= KillTimer(IDT_THINKING);
    BOOL bCardSending	= KillTimer(IDT_CARD_SENDING);
    BOOL bTokenTransfer	= KillTimer(IDT_TOKEN_TRANSFER);
    BOOL bCardSelect	= KillTimer(IDT_CARD_SELECT);
    BOOL bCardTransfer	= KillTimer(IDT_CARD_TRANSFER);

	CScoreDlg dlg(m_nScoreTotal, m_nScoreCur, this);
	dlg.DoModal();

	if(bDispatch)
		SetTimer(IDT_DISPATCH, TIMESLICE_DISPATCH, NULL);
	if(bCleanUp)
		SetTimer(IDT_CLEAN_UP, TIMESLICE_CLEAN_UP, NULL);
	if(bThinking)
		SetTimer(IDT_THINKING, m_nThinkingTimeslice, NULL);
	if(bCardSending)
		SetTimer(IDT_CARD_SENDING, TIMESLICE_CARD_SENDING, NULL);
	if(bTokenTransfer)
		SetTimer(IDT_TOKEN_TRANSFER, TIMESLICE_TOKEN_TRANSFER, NULL);
	if(bCardSelect)
		SetTimer(IDT_CARD_SELECT, TIMESLICE_CARD_SELECT, NULL);
	if(bCardTransfer)
		SetTimer(IDT_CARD_TRANSFER, TIMESLICE_CARD_TRANSFER, NULL);
}

void CChildView::OnAnimal() 
{
	// 关闭动画时只需要把动画标记置为FALSE即可，流程处理函数会自动根据该标记
	// 处理流程
	m_bAnimal = !m_bAnimal;
}

void CChildView::OnUpdateAnimal(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bAnimal);
}

void CChildView::OnHint() 
{
	// 提示
	// 从当前位置开始，搜索下一张可以打开的牌
	// 若没有可打出的牌，则取消弹出的牌

	int i;
	int start;
	int iCard;
	int iPopup;

	if(m_iPopup >= 0)
	{
		if(m_iPopup == m_PlayerCards[ID_PLAYER].size()-1)
			start = 0;
		else
			start = m_iPopup+1;
	}
	else
	{
		start = 0;
	}

	iPopup = -1;
	for(i=start; ; )
	{
		iCard = m_PlayerCards[ID_PLAYER][i].iCard;
		if(CanSendOut(iCard))
		{
			iPopup = i;
			break;
		}

		++i;
		if(i == m_PlayerCards[ID_PLAYER].size())
			i = 0;
		if(i==start)
			break;
	}

	if(iPopup < 0)
	{
		PopupUserCard(m_iPopup);

		m_bRedraw = TRUE;
		Invalidate(FALSE);
	}
	else if(iPopup != m_iPopup)
	{
		PopupUserCard(iPopup);

		m_bRedraw = TRUE;
		Invalidate(FALSE);
	}
}

void CChildView::OnSend() 
{
	ProcessUserSend(m_iPopup);
}

void CChildView::OnUpdateSend(CCmdUI* pCmdUI) 
{
	if (m_gs == GS_THINKING &&
		m_iCurrentTurn == ID_PLAYER &&
		m_iPopup >= 0 &&
		CanSendOut(m_PlayerCards[ID_PLAYER][m_iPopup].iCard) )
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CChildView::OnUpdateHint(CCmdUI* pCmdUI) 
{
	switch(m_gs)
	{
	case GS_PRESTART:
	case GS_DISPATCH:
	case GS_CLEAN_UP:
	case GS_END:
		pCmdUI->Enable(FALSE);
		break;

	default:
		pCmdUI->Enable(TRUE);
		break;
	}
}

void CChildView::OnTerminateGame() 
{
	if (m_gs != GS_PRESTART &&
		m_gs != GS_END)
	{
		m_gs = GS_END;

		KillTimer(IDT_DISPATCH);
		KillTimer(IDT_CLEAN_UP);
		KillTimer(IDT_THINKING);
		KillTimer(IDT_CARD_SENDING);
		KillTimer(IDT_TOKEN_TRANSFER);
		KillTimer(IDT_CARD_SELECT);
		KillTimer(IDT_CARD_TRANSFER);
	}
}

void CChildView::OnUpdateTerminateGame(CCmdUI* pCmdUI) 
{
	if (m_gs != GS_PRESTART &&
		m_gs != GS_END)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CChildView::OnSpeed(UINT nCmdID)
{
	switch(nCmdID)
	{
	case ID_SPEED_LOW:
		m_nThinkingTimeslice = TIMESLICE_LOW;
		break;

	case ID_SPEED_MID:
		m_nThinkingTimeslice = TIMESLICE_MID;
		break;

	case ID_SPEED_HIGH:
		m_nThinkingTimeslice = TIMESLICE_HIGH;
		break;

	default:
		ASSERT(0);
		break;
	}
}

void CChildView::OnUpdateSpeed(CCmdUI *pCmdUI)
{
	BOOL bRadio;

	switch(pCmdUI->m_nID)
	{
	case ID_SPEED_LOW:
		bRadio = (m_nThinkingTimeslice == TIMESLICE_LOW);
		break;

	case ID_SPEED_MID:
		bRadio = (m_nThinkingTimeslice == TIMESLICE_MID);
		break;

	case ID_SPEED_HIGH:
		bRadio = (m_nThinkingTimeslice == TIMESLICE_HIGH);
		break;

	default:
		ASSERT(0);
		break;
	}

	pCmdUI->SetRadio(bRadio);
}

void CChildView::OnSpeedUp() 
{
	if(m_nThinkingTimeslice == TIMESLICE_LOW)
	{
		m_nThinkingTimeslice = TIMESLICE_MID;
	}
	else if(m_nThinkingTimeslice == TIMESLICE_MID)
	{
		m_nThinkingTimeslice = TIMESLICE_HIGH;
	}
}

void CChildView::OnSpeedDown() 
{
	if(m_nThinkingTimeslice == TIMESLICE_HIGH)
	{
		m_nThinkingTimeslice = TIMESLICE_MID;
	}
	else if(m_nThinkingTimeslice == TIMESLICE_MID)
	{
		m_nThinkingTimeslice = TIMESLICE_LOW;
	}
}

void CChildView::OnAI(UINT nCmdID)
{
	switch(nCmdID)
	{
	case ID_DEGREE_EASY:
		m_ai = AI_EASY;
		break;

	case ID_DEGREE_NORMAL:
		m_ai = AI_NORMAL;
		break;

	case ID_DEGREE_HARD:
		m_ai = AI_HARD;
		break;

	default:
		ASSERT(0);
		break;
	}
}

void CChildView::OnUpdateAI(CCmdUI *pCmdUI)
{
	BOOL bRadio;

	switch(pCmdUI->m_nID)
	{
	case ID_DEGREE_EASY:
		bRadio = (m_ai == AI_EASY);
		break;

	case ID_DEGREE_NORMAL:
		bRadio = (m_ai == AI_NORMAL);
		break;

	case ID_DEGREE_HARD:
		bRadio = (m_ai == AI_HARD);
		break;

	default:
		ASSERT(0);
		break;
	}

	pCmdUI->SetRadio(bRadio);
}

///////////////////////////////////////////////////////////////////////////////
//	Mouse Message Handle
///////////////////////////////////////////////////////////////////////////////

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	int index = GetUserCardUnderPoint(point);
	if(index >= 0)
	{
		ProcessUserClick(index);
		return;
	}

	if ((m_gs == GS_THINKING || m_gs == GS_SELECT_CARD) &&
		m_iCurrentTurn == ID_PLAYER)
	{
		if(ProcessTokenHint(point))
			return;
	}
}

void CChildView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(m_iCurrentTurn==ID_PLAYER && !m_bUserAuto) // 非自动且轮到玩家
	{
		if(m_gs == GS_THINKING)
		{
			int index = GetUserCardUnderPoint(point); // 玩家点选的牌

			ProcessUserSend(index);
		}
		else if(m_gs == GS_SELECT_CARD)
		{
			int index = GetUserSelectCard(point);

			ProcessUserSelect(index);
		}
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	int index = GetUserCardUnderPoint(point);
	BeginSpecialShow(index);

	CWnd::OnRButtonDown(nFlags, point);
}

void CChildView::OnRButtonUp(UINT nFlags, CPoint point)
{
	EndSpecialShow();

	CWnd::OnRButtonUp(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
//	StatusBar Info Functions
///////////////////////////////////////////////////////////////////////////////

void CChildView::SetStatusBarInfo(LPCTSTR info)
{
	((CMainFrame *)(AfxGetApp()->GetMainWnd()))->UpdateInfo(0, info);
}

void CChildView::SetStatusBarInfo(int nID)
{
	CString info;
	info.LoadString(nID);

	((CMainFrame *)(AfxGetApp()->GetMainWnd()))->UpdateInfo(0, info);
}

void CChildView::SetWaitSendInfo()
{
	CString info, dir;

	switch(m_iCurrentTurn)
	{
	case ID_EAST:
	case ID_NORTH:
	case ID_WEST:
		dir.LoadString(IDS_PLAYER_EAST + (m_iCurrentTurn - ID_EAST));
		info.Format(IDS_PLAYER_THINKING, dir);
		break;

	case ID_SOUTH:
		info.LoadString(IDS_PLEASE_SEND_CARD);
		break;

	default:
		ASSERT(0);
		break;
	}

	SetStatusBarInfo(info);
}

void CChildView::SetSendInfo()
{
	CString info, dir, suit, value;

	dir.LoadString(IDS_PLAYER_EAST + (m_iCurrentTurn-ID_EAST));
	suit.LoadString(IDS_SUIT_0 + SUIT(m_iMovingCard));
	value.LoadString(IDS_VALUE_1 + VALUE(m_iMovingCard));

	info.Format(IDS_OUT_INFO, dir, suit, value);

	SetStatusBarInfo(info);
}

void CChildView::SetSelectInfo()
{
	CString info, dir;

	switch(m_iCurrentTurn)
	{
	case ID_EAST:
	case ID_NORTH:
	case ID_WEST:
		dir.LoadString(IDS_PLAYER_EAST + (m_iCurrentTurn - ID_EAST));
		info.Format(IDS_PLAYER_SELECT, dir);
		break;

	case ID_SOUTH:
		info.LoadString(IDS_PLEASE_SELECT_CARD);
		break;

	default:
		ASSERT(0);
		break;
	}

	SetStatusBarInfo(info);
}

void CChildView::SetSelectOverInfo()
{
	CString info, dir, suit, value;

	switch(m_iCurrentTurn)
	{
	case ID_EAST:
		suit.LoadString(IDS_SUIT_0 + SUIT(m_iMovingCard));
		value.LoadString(IDS_VALUE_1 + VALUE(m_iMovingCard));
		info.Format(IDS_EAST_SELECT_OVER, suit, value);
		break;

	case ID_NORTH:
	case ID_WEST:
		dir.LoadString(IDS_PLAYER_EAST + (m_iCurrentTurn - ID_EAST));
		info.Format(IDS_SELECT_OVER, dir);
		break;

	case ID_SOUTH:
		suit.LoadString(IDS_SUIT_0 + SUIT(m_iMovingCard));
		value.LoadString(IDS_VALUE_1 + VALUE(m_iMovingCard));
		info.Format(IDS_PLAYER_SELECT_OVER, suit, value);
		break;

	default:
		ASSERT(0);
		break;
	}

	SetStatusBarInfo(info);
}

void CChildView::SetCurrentTurnInfo()
{
	CString info, dir;

	dir.LoadString(IDS_PLAYER_EAST + ((m_iCurrentTurn+1)%PLAYER_NUM - ID_EAST));
	info.Format(IDS_IT_IS_YOUR_TURN, dir);

	SetStatusBarInfo(info);
}

///////////////////////////////////////////////////////////////////////////////
//	Other Functions
///////////////////////////////////////////////////////////////////////////////

void CChildView::InitProgram()
{
	GetClientRect(m_rcClient);

	// load images
	m_bmpCardShape.LoadBitmap(IDB_CARD_SHAPE);
	for(int i=0; i<CARD_NUM; i++)
	{
		m_bmpCard[i].LoadBitmap(IDB_CARD_01 + i);
		FitCardBitmap(m_bmpCard[i]);
	}
	m_bmpCardBack.LoadBitmap(IDB_CARD_BACK);
	FitCardBitmap(m_bmpCardBack);

	m_bmpBackground.LoadBitmap(IDB_BACKGROUND);
	m_bmpToken.LoadBitmap(IDB_TOKEN);
	m_bmpTokenShape.LoadBitmap(IDB_TOKEN_SHAPE);

	// clean player's cards
	for(int i=0; i<PLAYER_NUM; i++)
	{
		m_PlayerCards[i].resize(0);
	}

	// clean card lists
	for(int i=0; i<LIST_NUM; i++)
	{
		m_CardList[i].first = -1;
		m_CardList[i].last = -1;
	}

	// 分数复零
	for(int i=0; i<PLAYER_NUM; i++)
	{
		m_nScoreTotal[i] = 0;
		m_nScoreCur[i] = 0;
	}
}

void CChildView::Shuffle()
{
	int iCard[CARD_NUM];
	int s, d;

	// 把牌按顺序排列起来
	for(int i=0; i<CARD_NUM; i++)
	{
		iCard[i] = i;
	}

	// 模拟洗牌的过程
	s = 0;
	for(int i=0; i<CARD_NUM*4; i++)
	{
		d = rand()%CARD_NUM;
		iCard[s] = iCard[d];
		s = d;
	}
	iCard[s] = 0;

	// 把各个Player的牌清空
	for(int i=0; i<PLAYER_NUM; i++)
	{
		m_PlayerCards[i].clear();
	}

	// 把牌分发到各个Player
	ZCard card;
	for(int i=0; i<CARD_NUM; i++)
	{
		card.iCard = iCard[i];
		m_PlayerCards[i%4].push_back(card);
	}
}

void CChildView::CalcCardPos(int dir, BOOL bSuitMargin)
{
	ZPlayerCards &pc = m_PlayerCards[dir];
	const BOOL bFixedX = (dir==ID_EAST || dir==ID_WEST);
	const int nCardHeight = (bFixedX ? CARD_HEIGHT : CARD_WIDTH);
	const int nIntervalV = (bFixedX ? INTERVAL_V : INTERVAL_H);
	const int nIntervalSuitV = (bSuitMargin ? (bFixedX ? INTERVAL_SUIT_V : INTERVAL_SUIT_H) : nIntervalV);

	int nCard = pc.size();
	int x, y;
	int i;
	int t;

	int len = nCardHeight;
	for(i=1; i<nCard; i++)
	{
		if(SUIT(pc[i-1].iCard) == SUIT(pc[i].iCard))
			len += nIntervalV;
		else
			len += nIntervalSuitV;
	}

	switch(dir)
	{
	case ID_EAST:
		x = LEFT_EAST(m_rcClient.Width());
		y = (m_rcClient.Height() - len)/2;
		break;

	case ID_NORTH:
		x = (m_rcClient.Width() - len)/2;
		y = TOP_NORTH;
		break;

	case ID_WEST:
		x = LEFT_WEST;
		y = (m_rcClient.Height() - len)/2;
		break;

	case ID_SOUTH:
		x = (m_rcClient.Width() - len)/2;
		y = TOP_SOUTH(m_rcClient.Height());
		break;

	default:
		ASSERT(0);
		break;
	}

	for(i=0; ; )
	{
		pc[i].pos.x = x;
		pc[i].pos.y = y;

		if(++i >= pc.size())
			break;

		if(SUIT(pc[i-1].iCard) == SUIT(pc[i].iCard))
			t = nIntervalV;
		else
			t = nIntervalSuitV;

		if(bFixedX)
			y += t;
		else
			x += t;
	}

	if(dir == ID_PLAYER)
	{
		if(m_iPopup >= 0)
		{
			pc[m_iPopup].pos.y -= CARD_POPUP;
		}
	}
}

void CChildView::CalcCardPos(BOOL bSuitMargin)
{
	CalcCardPos(ID_EAST, bSuitMargin && m_bShowComputerCard);
	CalcCardPos(ID_NORTH, bSuitMargin && m_bShowComputerCard);
	CalcCardPos(ID_WEST, bSuitMargin && m_bShowComputerCard);
	CalcCardPos(ID_PLAYER, bSuitMargin);
}

void CChildView::CalcTokenStaticPos()
{
	m_ptTokenPos[ID_EAST] = m_PlayerCards[ID_EAST].front().pos +
		CPoint((CARD_WIDTH-TOKEN_WIDTH)/2, -INTERVAL_TOKEN-TOKEN_HEIGHT);
	m_ptTokenPos[ID_NORTH] = m_PlayerCards[ID_NORTH].front().pos +
		CPoint(-INTERVAL_TOKEN-TOKEN_WIDTH, (CARD_HEIGHT-TOKEN_HEIGHT)/2);
	m_ptTokenPos[ID_WEST] = m_PlayerCards[ID_WEST].back().pos +
		CPoint((CARD_WIDTH-TOKEN_WIDTH)/2, CARD_HEIGHT+INTERVAL_TOKEN);
	m_ptTokenPos[ID_SOUTH] = m_PlayerCards[ID_SOUTH].back().pos +
		CPoint(CARD_WIDTH+INTERVAL_TOKEN, (CARD_HEIGHT-TOKEN_HEIGHT)/2);
}

void CChildView::CleanUpCards()
{
	for(int i=0; i<PLAYER_NUM; i++)
	{
		sort(m_PlayerCards[i].begin(), m_PlayerCards[i].end());
	}

	CalcCardPos(TRUE);
}

int CChildView::FindFirstOut()
{
	int i;
	ZPlayerCards::iterator iter;

	for(i=0; i<PLAYER_NUM; i++)
	{
		for(iter=m_PlayerCards[i].begin(); iter!=m_PlayerCards[i].end(); iter++)
		{
			if(iter->iCard == FIRST_OUT_CARD)
				return i;
		}
	}

	ASSERT(0);
	return -1;
}

void CChildView::SendCard(int index)
{
	ZPlayerCards &pc = m_PlayerCards[m_iCurrentTurn];
	int iCard = pc[index].iCard;

	m_iMovingCard = iCard;
	m_ptMovingCard = pc[index].pos;
	m_ptMovingCardSource = m_ptMovingCard;

	m_ptMovingCardDest.x =
		LEFT_LIST(m_rcClient.Width()) + (SUIT_NUM-1-SUIT(iCard))*LIST_INTERVAL_H;
	m_ptMovingCardDest.y =
		TOP_LIST(m_rcClient.Height()) + VALUE(iCard)*LIST_INTERVAL_V;

	pc.erase(pc.begin()+index);

	if(m_iCurrentTurn == ID_PLAYER)
	{
		m_iPopup = -1;

		if(m_iSpecialShow >=0 )
		{
			m_iSpecialShow = -1;
			ReleaseCapture();
		}
	}

	// do not call CalcCardPos() here
}

BOOL CChildView::IsGameOver()
{
	for(int i=0; i<PLAYER_NUM; i++)
	{
		if(m_PlayerCards[i].size() == 0)
			return TRUE;
	}

	return FALSE;
}

void CChildView::CalcMovingCardPos(int nMovingNum)
{
	m_ptMovingCard.x =
		(m_ptMovingCardSource.x*(nMovingNum-m_iAnimalCount) + m_ptMovingCardDest.x*m_iAnimalCount) / nMovingNum;
	m_ptMovingCard.y =
		(m_ptMovingCardSource.y*(nMovingNum-m_iAnimalCount) + m_ptMovingCardDest.y*m_iAnimalCount) / nMovingNum;
}

void CChildView::AcceptSendingCard()
{
	int iSuit = SUIT(m_iMovingCard);

	if(VALUE(m_iMovingCard) == CENTER_CARD)
	{
		m_CardList[iSuit].first = m_iMovingCard;
		m_CardList[iSuit].last = m_iMovingCard;
	}
	else
	{
		if(m_iMovingCard == m_CardList[iSuit].first-1)
			m_CardList[iSuit].first--;
		else if(m_iMovingCard == m_CardList[iSuit].last+1)
			m_CardList[iSuit].last++;
		else
			ASSERT(0);
	}
}

int CChildView::CardCanOut()
{
	int count = 0;

	for(int i=0; i<m_PlayerCards[m_iCurrentTurn].size(); i++)
	{
		if(CanSendOut(m_PlayerCards[m_iCurrentTurn][i].iCard))
			++count;
	}

	return count;
}

void CChildView::CalcTokenPos()
{
	if(m_gs == GS_TRANSFER_TOKEN)
	{
		CPoint ptSource = m_ptTokenPos[m_iCurrentTurn];
		CPoint ptDest = m_ptTokenPos[(m_iCurrentTurn+1)%PLAYER_NUM];

		m_ptToken.x = (ptSource.x * (TOKEN_TRANSFER_NUM-m_iAnimalCount) + ptDest.x * m_iAnimalCount) / TOKEN_TRANSFER_NUM;
		m_ptToken.y = (ptSource.y * (TOKEN_TRANSFER_NUM-m_iAnimalCount) + ptDest.y * m_iAnimalCount) / TOKEN_TRANSFER_NUM;
	}
	else
	{
		m_ptToken = m_ptTokenPos[m_iCurrentTurn];
	}
}

void CChildView::ShufflePlayerCards(int dir)
{
	ZPlayerCards &pc = m_PlayerCards[dir];

	int s, d;
	int front = pc.front().iCard;
	int nCard = pc.size();

	// 模拟洗牌的过程
	s = 0;
	for(int i=0; i<nCard*4; i++)
	{
		d = rand()%nCard;
		pc[s].iCard = pc[d].iCard;
		s = d;
	}
	pc[s].iCard = front;
}

CRect CChildView::GetCardMovingInvalidRect(int nMovingNum)
{
	CRect rcCardOrigin, rcCardDest;
	CRect rcInvalid;
	int i;

	// rcCardOrigin
	i = m_iAnimalCount-1;
	rcCardOrigin.left =
		(m_ptMovingCardSource.x*(nMovingNum-i) + m_ptMovingCardDest.x*i) / nMovingNum;
	rcCardOrigin.top =
		(m_ptMovingCardSource.y*(nMovingNum-i) + m_ptMovingCardDest.y*i) / nMovingNum;
	rcCardOrigin.right = rcCardOrigin.left + CARD_WIDTH;
	rcCardOrigin.bottom = rcCardOrigin.top + CARD_HEIGHT;

	// rcCardDest
	i = m_iAnimalCount;
	rcCardDest.left =
		(m_ptMovingCardSource.x*(nMovingNum-i) + m_ptMovingCardDest.x*i) / nMovingNum;
	rcCardDest.top =
		(m_ptMovingCardSource.y*(nMovingNum-i) + m_ptMovingCardDest.y*i) / nMovingNum;
	rcCardDest.right = rcCardDest.left + CARD_WIDTH;
	rcCardDest.bottom = rcCardDest.top + CARD_HEIGHT;

	// invalid rect
	rcInvalid.UnionRect(rcCardOrigin, rcCardDest);

	return rcInvalid;
}

CRect CChildView::GetTokenTransferInvalidRect()
{
	CPoint ptSource, ptDest;
	CRect rcTokenOrigin, rcTokenDest;
	CRect rcInvalid;
	int i;

	ptSource = m_ptTokenPos[m_iCurrentTurn];
	ptDest = m_ptTokenPos[(m_iCurrentTurn+1)%PLAYER_NUM];

	// rcTokenOrigin
	i = m_iAnimalCount-1;
	rcTokenOrigin.left =
		(ptSource.x*(TOKEN_TRANSFER_NUM-i) + ptDest.x*i) / TOKEN_TRANSFER_NUM;
	rcTokenOrigin.top =
		(ptSource.y*(TOKEN_TRANSFER_NUM-i) + ptDest.y*i) / TOKEN_TRANSFER_NUM;
	rcTokenOrigin.right = rcTokenOrigin.left + TOKEN_WIDTH;
	rcTokenOrigin.bottom = rcTokenOrigin.top + TOKEN_HEIGHT;

	// rcTokenDest
	i = m_iAnimalCount;
	rcTokenDest.left =
		(ptSource.x*(TOKEN_TRANSFER_NUM-i) + ptDest.x*i) / TOKEN_TRANSFER_NUM;
	rcTokenDest.top =
		(ptSource.y*(TOKEN_TRANSFER_NUM-i) + ptDest.y*i) / TOKEN_TRANSFER_NUM;
	rcTokenDest.right = rcTokenDest.left + TOKEN_WIDTH;
	rcTokenDest.bottom = rcTokenDest.top + TOKEN_HEIGHT;

	// invalid rect
	rcInvalid.UnionRect(rcTokenOrigin, rcTokenDest);

	return rcInvalid;
}

void CChildView::UpdateRemainInfo()
{
	CMainFrame *pMainWnd = (CMainFrame *)(AfxGetApp()->GetMainWnd());
	CString info, dir;

	for(int i=0; i<PLAYER_NUM; i++)
	{
		dir.LoadString(IDS_PLAYER_EAST + (i-ID_EAST));

		if(i != ID_PLAYER)
		{
			info.Format(
				IDS_REMAIN_CARD_FORMAT,
				dir,
				m_PlayerCards[i].size());
		}
		else
		{
			info.Format(
				IDS_USER_CARD_FORMAT,
				dir,
				m_PlayerCards[i].size(),
				CScoreDlg::GetRankString(m_nScoreTotal));
		}

		pMainWnd->UpdateInfo(i+1, info);
	}
}

int CChildView::GetUserCardUnderPoint(CPoint point)
{
	const ZPlayerCards &pc = m_PlayerCards[ID_PLAYER];
	int i;
	CRect rect;

	for(i=pc.size()-1; i>=0; i--)
	{
		rect.left	= pc[i].pos.x;
		rect.right	= rect.left + CARD_WIDTH;
		rect.top	= pc[i].pos.y;
		rect.bottom	= rect.top + CARD_HEIGHT;

		if(rect.PtInRect(point))
			break;
	}

	return i;
}

BOOL CChildView::CanSendOut(int iCard)
{
	if(VALUE(iCard) == CENTER_CARD) // 是中间的牌（即是七）
	{
		return TRUE;
	}
	else
	{
		int iSuit = SUIT(iCard);
		if(m_CardList[iSuit].first >= 0 && 
		   (iCard == m_CardList[iSuit].first-1 ||
			iCard == m_CardList[iSuit].last+1 ) )
		{
			return TRUE;
		}
	}

	return FALSE;
}

int CChildView::FindPlayerCard(int id, int iCard)
{
	const ZPlayerCards &pc = m_PlayerCards[id];
	int i;

	for(i=pc.size()-1; i>=0; --i)
	{
		if(pc[i].iCard == iCard)
			break;
	}

	return i;
}

BOOL CChildView::ProcessUserSend(int index)
{
	if (m_gs != GS_THINKING ||
		index < 0 ||
		m_iCurrentTurn != ID_PLAYER)
	{
		return FALSE;
	}

	int iCard = m_PlayerCards[ID_PLAYER][index].iCard;

	if(CanSendOut(iCard))
	{
		// 选中的牌可以打出

		if(iCard!=FIRST_OUT_CARD && FindPlayerCard(ID_PLAYER, FIRST_OUT_CARD)>=0)
		{
			// 有黑桃七 但是 没有打出
			SetStatusBarInfo(IDS_MUST_SEND_45);
			return FALSE;
		}
		else
		{
			// 正常出牌
			m_iPopup = -1;
			m_iSpecialShow = -1;

			SendCard(index);
			ThinkingOver();
			return TRUE;
		}
	}
	else
	{
		// 选中的牌不可以打出
		SetStatusBarInfo(IDS_CANNOT_SEND_CARD);
		return FALSE;
	}
}

int CChildView::GetUserSelectCard(CPoint point)
{
	const ZPlayerCards &pc = m_PlayerCards[ID_WEST];
	int i;
	CRect rect;

	for(i=pc.size()-1; i>=0; i--)
	{
		rect.left	= pc[i].pos.x;
		rect.right	= rect.left + CARD_WIDTH;
		rect.top	= pc[i].pos.y;
		rect.bottom	= rect.top + CARD_HEIGHT;

		if(rect.PtInRect(point))
			break;
	}

	return i;
}

BOOL CChildView::ProcessUserSelect(int index)
{
	if(index < 0) // 没有选中了牌
		return FALSE;

	m_iPopup = -1;
	m_iSpecialShow = -1;

	m_iSelectCardIndex = index;
	CardSelectOver();

	return TRUE;
}

BOOL CChildView::ProcessUserClick(int index)
{
	PopupUserCard(index);

	m_bRedraw = TRUE;
	Invalidate(FALSE);

	return TRUE;
}

void CChildView::BeginSpecialShow(int index)
{
	if(index < 0)
		return;

	m_iSpecialShow = index;

	SetCapture();

	m_bRedraw = TRUE;
	Invalidate(FALSE);
}

void CChildView::EndSpecialShow()
{
	if(m_iSpecialShow >= 0)
	{
		m_iSpecialShow = -1;

		ReleaseCapture();

		m_bRedraw = TRUE;
		Invalidate(FALSE);
	}
}

void CChildView::ProcessGameOver()
{
	m_bRedraw = TRUE;
	Invalidate(FALSE);

	for(int i=0; i<PLAYER_NUM; i++)
	{
		int nCardOut = CARD_EACH_INIT - m_PlayerCards[i].size();

		if(nCardOut >= 0)
		{
			m_nScoreCur[i] = (nCardOut+1)*nCardOut/2;
		}
		else
		{
			m_nScoreCur[i] = nCardOut * PUNISH_PER_CARD;
		}
		m_nScoreTotal[i] += m_nScoreCur[i];
	}

	if(!m_bUserAuto) // 非电脑托管
	{
		OnScore();
	}

	PostMessage(WM_COMMAND, ID_NEW_GAME, 0);
}

void CChildView::PopupUserCard(int index)
{
	if(index < 0)
		return;

	if(m_iPopup == -1)
	{
		m_PlayerCards[ID_PLAYER][index].pos.y -= CARD_POPUP;
		m_iPopup = index;
	}
	else if(m_iPopup == index)
	{
		m_PlayerCards[ID_PLAYER][index].pos.y += CARD_POPUP;
		m_iPopup = -1;
	}
	else
	{
		m_PlayerCards[ID_PLAYER][m_iPopup].pos.y += CARD_POPUP;
		m_PlayerCards[ID_PLAYER][index].pos.y -= CARD_POPUP;
		m_iPopup = index;
	}
}

void CChildView::ErrorReport(LPCTSTR lpszFileName, int nLine)
{
	CString string, title;
	string.Format(IDS_ASSERT_ERROR, lpszFileName, nLine);
	title.LoadString(AFX_IDS_APP_TITLE);

	MessageBox(string, title, MB_OK | MB_ICONERROR);

	PostQuitMessage(0);
}

BOOL CChildView::ProcessTokenHint(CPoint point)
{
	CRect rect;
	rect.SetRect(
		m_ptTokenPos[ID_PLAYER], 
		m_ptTokenPos[ID_PLAYER] + CSize(TOKEN_WIDTH, TOKEN_HEIGHT));

	if(rect.PtInRect(point))
	{
		OnHint();
		return TRUE;
	}

	return FALSE;
}
