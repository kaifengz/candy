// ChildView.h : interface of the CChildView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__306E70C5_EFB9_49CE_BFC5_F32F3D0C6442__INCLUDED_)
#define AFX_CHILDVIEW_H__306E70C5_EFB9_49CE_BFC5_F32F3D0C6442__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using std::vector;

#include "defines.h"

/////////////////////////////////////////////////////////////////////////////
// CChildView window

class CChildView : public CWnd
{
// Construction
public:
	CChildView();
	virtual ~CChildView();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
	afx_msg void OnNewGame();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowComputerCard();
	afx_msg void OnPlayerAuto();
	afx_msg void OnUpdatePlayerAuto(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point); 
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnScore();
	afx_msg void OnAnimal();
	afx_msg void OnUpdateAnimal(CCmdUI* pCmdUI);
	afx_msg void OnHint();
	afx_msg void OnSend();
	afx_msg void OnUpdateSend(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHint(CCmdUI* pCmdUI);
	afx_msg void OnTerminateGame();
	afx_msg void OnUpdateTerminateGame(CCmdUI* pCmdUI);
	afx_msg void OnSpeed(UINT nCmdID);
	afx_msg void OnUpdateSpeed(CCmdUI* pCmdUI);
	afx_msg void OnSpeedUp();
	afx_msg void OnSpeedDown();
	afx_msg void OnAI(UINT nCmdID);
	afx_msg void OnUpdateAI(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	enum ZGameStatus
	{
		GS_PRESTART,		// 还没有开始
		GS_DISPATCH,		// 发牌
		GS_CLEAN_UP,		// 电脑、玩家整理牌
		GS_THINKING,		// 电脑、玩家思考中
		GS_SENDING_CARD,	// 出牌
		GS_TRANSFER_TOKEN,	// 传送令牌
		GS_END,				// 牌局结束
		GS_SELECT_CARD,		// 下家从上家的牌中挑选一张牌
		GS_TRANSFER_CARD,	// 上家的牌传送至下家
	};

	enum ZAIDegree
	{
		AI_EASY,
		AI_NORMAL,
		AI_HARD,
	};

	struct ZCard
	{
		int iCard;
		CPoint pos;

		int operator<(const ZCard &c)
		{
			if(iCard/SUIT_EACH != c.iCard/SUIT_EACH)
				return iCard/SUIT_EACH > c.iCard/SUIT_EACH;
			else
				return iCard < c.iCard;
		}
	};

	typedef vector<ZCard> ZPlayerCards;

	struct ZCardList
	{
		int first;
		int last;
	};

private:
	// game attributes
	ZGameStatus		m_gs;
	int				m_iCurrentTurn;
	int				m_iPopup;
	int				m_iSpecialShow;
	int				m_iAnimalCount;

	// token
	CPoint			m_ptToken;
	CPoint			m_ptTokenPos[PLAYER_NUM];

	// moving card
	int				m_iMovingCard;
	CPoint			m_ptMovingCard;
	CPoint			m_ptMovingCardSource;
	CPoint			m_ptMovingCardDest;
	int				m_iSelectCardIndex;

	// player score
	int				m_nScoreTotal[PLAYER_NUM];
	int				m_nScoreCur[PLAYER_NUM];

	// card index
	ZPlayerCards	m_PlayerCards[PLAYER_NUM];
	ZCardList		m_CardList[LIST_NUM];

	// drawing
	CRect			m_rcClient;
	CBitmap			*m_pBmpClient;
	BOOL			m_bRedraw;

	// options
	BOOL			m_bAnimal;
	BOOL			m_bUserAuto;
	BOOL			m_bShowComputerCard;
	UINT			m_nThinkingTimeslice;
	ZAIDegree		m_ai;

	// bitmap resource
	CBitmap			m_bmpBackground;
	CBitmap			m_bmpCard[CARD_NUM];
	CBitmap			m_bmpCardBack;
	CBitmap			m_bmpCardShape;
	CBitmap			m_bmpToken;
	CBitmap			m_bmpTokenShape;

private:
	void InitProgram();
	void Shuffle();
	void CleanUpCards();
	int  FindFirstOut();
	void SendCard(int index);
	int  ComputerThink();
	BOOL IsGameOver();
	void AcceptSendingCard();
	int  CardCanOut();
	void ShufflePlayerCards(int dir);
	BOOL CanSendOut(int iCard);
	int  FindPlayerCard(int id, int iCard);
	void PopupUserCard(int index);

	void ProcessGameOver();

	// user special
	int  GetUserCardUnderPoint(CPoint point);	// 选择玩家自己的牌
	int  GetUserSelectCard(CPoint point);		// 选择上家的牌
	BOOL ProcessUserSend(int index);			// 玩家出牌
	BOOL ProcessUserSelect(int index);			// 玩家拿走上家的牌
	BOOL ProcessUserClick(int index);			// 玩家点选自己的牌
	void BeginSpecialShow(int index);			// 
	void EndSpecialShow();						// 
	BOOL ProcessTokenHint(CPoint point);		// 玩家单击灵牌寻求帮助

	// AI
	int  AIFirstOut();								// 第一张牌必须出黑桃七
	int  AIEasy(const vector<int> &legal_out);		// 随机的打开可以打出的牌
	int  AINormal(const vector<int> &legal_out);	// 尽可能的不减少自己可打出牌的数目和不增加敌手可打出牌的数目
	int  AIHard(const vector<int> &legal_out);		// 

	// debug
	void ErrorReport(LPCSTR lpszFileName, int nLine);

	// status bar info
	void UpdateRemainInfo();
	void SetStatusBarInfo(LPCTSTR info);
	void SetStatusBarInfo(int nID);
	void SetWaitSendInfo();
	void SetSendInfo();
	void SetSelectInfo();
	void SetSelectOverInfo();
	void SetCurrentTurnInfo();

	// invalid rect
	CRect GetCardMovingInvalidRect(int nMovingNum);
	CRect GetTokenTransferInvalidRect();

	// card and token position
	void CalcCardPos(int dir, BOOL bSuitMargin);
	void CalcCardPos(BOOL bSuitMargin);
	void CalcMovingCardPos(int nMovingNum);
	void CalcTokenStaticPos();
	void CalcTokenPos();

	// drawing
	void DrawAll(CDC *pDC);
	void DrawCard(CDC *pDC, CBitmap *pBmpCard, int x, int y);
	void DrawToken(CDC *pDC);
	void FitCardBitmap(CBitmap &bitmap);

private:
	// 初始化，启动发牌定时器，若动画关闭则直接调用DispathOver
	void NewGame();

	// 发牌结束，启动整理牌定时器，若动画关闭则整理牌（即把牌排序）
	// 后直接调用CleanUpOver
	void DispatchOver();

	// 整理牌结束，寻找首先出牌者，则启动思考定时器（或等待玩家的鼠
	// 标输入）
	void CleanUpOver();

	// 思考结束（或玩家鼠标已经正确输入），启动出牌定时器，若动画关
	// 闭则直接调用CardSendingOver
	void ThinkingOver();

	// 出牌结束，判断是否GameOver，若是则启动发牌定时器开始下一局，
	// 否则启动传送令牌定时器，若动画关闭则直接调用TokenTransferOver
	void CardSendingOver();

	// 令牌传送结束，若正准备出牌的这一家有牌可出，则启动思考定时器；
	// 若无牌可出，则启动选择牌定时器（或等待玩家的鼠标输入）
	void TokenTransferOver();

	// 电脑选择上家的牌结束（或玩家鼠标已经正确输入），启动传送牌定
	// 时器
	void CardSelectOver();

	// 牌传送结束，判断是否GameOver，若是则启动发牌定时器开始下一局，
	// 否则启动传送令牌定时器，若动画关闭则直接调用TokenTransferOver
	void CardTransferOver();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__306E70C5_EFB9_49CE_BFC5_F32F3D0C6442__INCLUDED_)
