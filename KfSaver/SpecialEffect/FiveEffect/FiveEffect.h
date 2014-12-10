
#ifndef _FIVE_EFFECT_H__INCLUDED__2006_08_10_
#define _FIVE_EFFECT_H__INCLUDED__2006_08_10_

#include "SpecialEffect.h"

class CFiveEffect : public CSpecialEffect  
{
public:
	CFiveEffect(HWND, HBITMAP);
	virtual ~CFiveEffect();

protected:
	virtual const char* GetEffectName() const;
    virtual BOOL Initialize(HDC, BOOL);
	virtual BOOL OnTimer(HDC);
	virtual DWORD GetTimeslice() const;

private:
	struct ZFivePoint
	{
		unsigned short x, y;
		ZFivePoint(unsigned short _x=0, unsigned short _y=0) : x(_x), y(_y) {}
	};
	typedef ZFivePoint ZFP;
	typedef vector<ZFivePoint> ZFivePointArray, ZFPA;

	struct ZFiveGameStored
	{
		ZFivePointArray steps;
 		// BOOL bBlackWin;
		// may have more informations, such that game time, game competitors, etc
	};
	typedef ZFiveGameStored ZFGS;
	typedef vector<ZFiveGameStored> ZFiveGameLib;

	enum ZFiveGameState
	{
		FGS_WAIT_TIP = 0,
		FGS_WAIT_NEXT,
		FGS_WAIT_END,
		FGS_ENDED,

		FGS_MAX,
	};

	struct ZFiveGame : ZFiveGameStored
	{
		ZFiveGameState state;
		unsigned int nCurrentStep;
		unsigned int nWaitTipCount;
		unsigned int nWaitNextCount;
		unsigned int nWaitEndCount;
	};

private:
	HBITMAP m_hBmpBoard, m_hBmpBlack, m_hBmpWhite;
	ZFiveGame m_game;

	unsigned int m_nGridSize;
	unsigned int m_xMargin, m_yMargin;

private:
	BOOL LoadImages(HDC);
	void ReleaseImages();

	BOOL LoadGame();
	static BOOL LoadGameLib(LPCSTR lpResource, ZFiveGameLib &glib);
	static BOOL LoadGameLib(LPCSTR lpResource, DWORD nSize, ZFiveGameLib &glib);
	void ReleaseGame();

	void DrawAll(HDC);
	void DrawBoard(HDC);
	void DrawChessman(HDC, const ZFivePoint&, BOOL bBlack);
	void DrawNextTip(HDC, const ZFivePoint&);

	BOOL NextStep();
};

#endif // _FIVE_EFFECT_H__INCLUDED__2006_08_10_
