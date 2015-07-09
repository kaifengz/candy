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
		GS_PRESTART,		// ��û�п�ʼ
		GS_DISPATCH,		// ����
		GS_CLEAN_UP,		// ���ԡ����������
		GS_THINKING,		// ���ԡ����˼����
		GS_SENDING_CARD,	// ����
		GS_TRANSFER_TOKEN,	// ��������
		GS_END,				// �ƾֽ���
		GS_SELECT_CARD,		// �¼Ҵ��ϼҵ�������ѡһ����
		GS_TRANSFER_CARD,	// �ϼҵ��ƴ������¼�
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
	int  GetUserCardUnderPoint(CPoint point);	// ѡ������Լ�����
	int  GetUserSelectCard(CPoint point);		// ѡ���ϼҵ���
	BOOL ProcessUserSend(int index);			// ��ҳ���
	BOOL ProcessUserSelect(int index);			// ��������ϼҵ���
	BOOL ProcessUserClick(int index);			// ��ҵ�ѡ�Լ�����
	void BeginSpecialShow(int index);			// 
	void EndSpecialShow();						// 
	BOOL ProcessTokenHint(CPoint point);		// ��ҵ�������Ѱ�����

	// AI
	int  AIFirstOut();								// ��һ���Ʊ����������
	int  AIEasy(const vector<int> &legal_out);		// ����Ĵ򿪿��Դ������
	int  AINormal(const vector<int> &legal_out);	// �����ܵĲ������Լ��ɴ���Ƶ���Ŀ�Ͳ����ӵ��ֿɴ���Ƶ���Ŀ
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
	// ��ʼ�����������ƶ�ʱ�����������ر���ֱ�ӵ���DispathOver
	void NewGame();

	// ���ƽ��������������ƶ�ʱ�����������ر��������ƣ�����������
	// ��ֱ�ӵ���CleanUpOver
	void DispatchOver();

	// �����ƽ�����Ѱ�����ȳ����ߣ�������˼����ʱ������ȴ���ҵ���
	// �����룩
	void CleanUpOver();

	// ˼�����������������Ѿ���ȷ���룩���������ƶ�ʱ������������
	// ����ֱ�ӵ���CardSendingOver
	void ThinkingOver();

	// ���ƽ������ж��Ƿ�GameOver���������������ƶ�ʱ����ʼ��һ�֣�
	// ���������������ƶ�ʱ�����������ر���ֱ�ӵ���TokenTransferOver
	void CardSendingOver();

	// ���ƴ��ͽ���������׼�����Ƶ���һ�����ƿɳ���������˼����ʱ����
	// �����ƿɳ���������ѡ���ƶ�ʱ������ȴ���ҵ�������룩
	void TokenTransferOver();

	// ����ѡ���ϼҵ��ƽ��������������Ѿ���ȷ���룩�����������ƶ�
	// ʱ��
	void CardSelectOver();

	// �ƴ��ͽ������ж��Ƿ�GameOver���������������ƶ�ʱ����ʼ��һ�֣�
	// ���������������ƶ�ʱ�����������ر���ֱ�ӵ���TokenTransferOver
	void CardTransferOver();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__306E70C5_EFB9_49CE_BFC5_F32F3D0C6442__INCLUDED_)
