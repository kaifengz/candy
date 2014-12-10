
#ifndef _CHEVY_EFFECT_H__INCLUDED__2006_08_16_
#define _CHEVY_EFFECT_H__INCLUDED__2006_08_16_

#include "SpecialEffect.h"

namespace NChevy {
	class CChevyScene;
}

class CChevyEffect : public CSpecialEffect  
{
public:
	CChevyEffect(HWND, HBITMAP);
	virtual ~CChevyEffect();

protected:
	virtual const char* GetEffectName() const;
	virtual BOOL Initialize(HDC, BOOL);
	virtual BOOL OnTimer(HDC);
	virtual DWORD GetTimeslice() const;

private:
	NChevy::CChevyScene *m_pScene;
};

#endif // _CHEVY_EFFECT_H__INCLUDED__2006_08_16_
