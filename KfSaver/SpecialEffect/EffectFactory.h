
#ifndef _EFFECT_FACTORY_H__INCLUDED__2005_01_03_
#define _EFFECT_FACTORY_H__INCLUDED__2005_01_03_

#include "SpecialEffect.h"

class CEffectFactory  
{
public:
	static CSpecialEffect *CreateEffect(HWND hWnd,
										HBITMAP hBmpBkgnd);

	static void DeleteEffect(CSpecialEffect *&);
};

#endif // _EFFECT_FACTORY_H__INCLUDED__2005_01_03_
