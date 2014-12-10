
#ifndef MAGNIFIER_EFFECT_H__INCLUDED__2005_01_03_
#define MAGNIFIER_EFFECT_H__INCLUDED__2005_01_03_

#include "SpecialEffect.h"

class CMagnifierEffect : public CSpecialEffect  
{
public:
	CMagnifierEffect(HWND, HBITMAP);
	virtual ~CMagnifierEffect();

protected:
	virtual const char* GetEffectName() const;
    virtual BOOL Initialize(HDC, BOOL);
	virtual BOOL OnTimer(HDC);
	virtual BOOL MagicHong(HDC);

private:
	int m_x, m_y;
	int m_vx, m_vy;

	int m_nRadius;
};


#endif // MAGNIFIER_EFFECT_H__INCLUDED__2005_01_03_
