
#ifndef _OTHELLO_EFFECT_H__INCLUDED__2007_05_04_
#define _OTHELLO_EFFECT_H__INCLUDED__2007_05_04_

#include "SpecialEffect.h"

namespace NOthello {
	class COthello;
	class COthelloBoard;
}

///////////////////////////////////////////////////////////////////////////////
//	COthelloEffect
///////////////////////////////////////////////////////////////////////////////

class COthelloEffect : public CSpecialEffect
{
public:
	COthelloEffect(HWND hWnd, HBITMAP hBmpBkgnd);
	virtual ~COthelloEffect();

protected:
	virtual const char* GetEffectName() const;
	virtual BOOL Initialize(HDC, BOOL);
	virtual BOOL OnTimer(HDC);

protected:
	void DrawBoard(HDC);

private:
	NOthello::COthello *m_pOthello;

	enum {
		WAITING = 0,
		THINKING,
		SWITCHING,
		TERMINATED,
	} m_state;
	DWORD m_nCount;
	NOthello::COthelloBoard *m_pPrevBoard;
	NOthello::COthelloBoard *m_pCurrBoard;
};

#endif // _OTHELLO_EFFECT_H__INCLUDED__2007_05_04_
