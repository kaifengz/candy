
#ifndef _BUMPER_EFFECT_H__INCLUDED__2007_06_27_
#define _BUMPER_EFFECT_H__INCLUDED__2007_06_27_

#include "SpecialEffect.h"

namespace NBumper {
	class CBumperScene;
}

class CBumperEffect : public CSpecialEffect  
{
public:
	CBumperEffect(HWND, HBITMAP);
	virtual ~CBumperEffect();

protected:
	virtual const char* GetEffectName() const;
	virtual BOOL Initialize(HDC, BOOL);
	virtual BOOL OnTimer(HDC);
	virtual DWORD GetTimeslice() const;

private:
	NBumper::CBumperScene *m_pScene;
};

#endif // _BUMPER_EFFECT_H__INCLUDED__2007_06_27_
