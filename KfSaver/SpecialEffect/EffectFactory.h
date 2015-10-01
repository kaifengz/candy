
#ifndef _EFFECT_FACTORY_H__INCLUDED__2005_01_03_
#define _EFFECT_FACTORY_H__INCLUDED__2005_01_03_

#include "SpecialEffect.h"

class CEffectCreationStrategy
{
public:
	virtual ~CEffectCreationStrategy() {}
	virtual const char* GetStrategyName() const = 0;
	virtual CSpecialEffect* CreateEffect(HWND hWnd, HBITMAP hBmpBkgnd) = 0;
};

enum ZEffectCreationStrategy
{
	S_RANDOM_CONFIGURED = 0,
	S_RANDOM_ALL,
	S_SEQUENTIAL_CONFIGURED,
	S_SEQUENTIAL_ALL,
	S_LATEST_FOR_TEST,
};

class CEffectFactory  
{
public:
	static CSpecialEffect *CreateEffect(HWND hWnd,
										HBITMAP hBmpBkgnd);

	static void DeleteEffect(CSpecialEffect *&);

	static void Initialize();

	static BOOL SetCreationStrategy(ZEffectCreationStrategy strategy);

private:
	static CEffectCreationStrategy *m_strategy;
};

#endif // _EFFECT_FACTORY_H__INCLUDED__2005_01_03_
