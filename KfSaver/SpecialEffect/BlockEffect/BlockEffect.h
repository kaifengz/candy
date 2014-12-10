
#ifndef _BLOCK_EFFECT_H__INCLUDED__2005_01_23_
#define _BLOCK_EFFECT_H__INCLUDED__2005_01_23_

#include "SpecialEffect.h"

namespace NBlock {
	class CBlockScene;
}

class CBlockEffect : public CSpecialEffect  
{
public:
	CBlockEffect(HWND hWnd, HBITMAP hBmpBkgnd);
	virtual ~CBlockEffect();

protected:
	virtual const char* GetEffectName() const;
    virtual BOOL Initialize(HDC, BOOL);
	virtual BOOL OnTimer(HDC);

private:
	NBlock::CBlockScene *m_pScene;
};

#endif // _BLOCK_EFFECT_H__INCLUDED__2005_01_23_
