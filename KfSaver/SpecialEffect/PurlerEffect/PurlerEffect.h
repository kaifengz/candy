
#ifndef _PURLER_EFFECT_H__INCLUDED__2006_10_01_
#define _PURLER_EFFECT_H__INCLUDED__2006_10_01_

#include "SpecialEffect.h"

namespace NPurler {
	class CPurlerScene;
}

class CPurlerEffect : public CSpecialEffect  
{
public:
	CPurlerEffect(HWND, HBITMAP);
	virtual ~CPurlerEffect();

protected:
	virtual const char* GetEffectName() const;
    virtual BOOL Initialize(HDC, BOOL);
	virtual BOOL OnTimer(HDC);
	virtual DWORD GetTimeslice() const;

private:
	NPurler::CPurlerScene *m_pScene;
};

#endif // _PURLER_EFFECT_H__INCLUDED__2006_10_01_
