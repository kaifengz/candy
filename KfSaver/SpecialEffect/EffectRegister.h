
#ifndef _EFFECT_REGISTER_H__INCLUDED__2008_05_17_
#define _EFFECT_REGISTER_H__INCLUDED__2008_05_17_

#include "SpecialEffect.h"
#include <map>

class EffectRegistry
{
public:
	typedef CSpecialEffect* (*CreateEffectFunc)(HWND hWnd, HBITMAP hBmpBkgnd);
	static void RegisterEffect(CreateEffectFunc func, const char* name);

	typedef std::vector<CreateEffectFunc> EffectCreators;
	static const EffectCreators& GetEffectCreators();
	static const EffectCreators& GetConfiguredEffectCreators();

	static void RegisterEffectForTest(CreateEffectFunc func, const char* name, const char* date);
	static CreateEffectFunc GetCreatorForTest();
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

template<class SpecialEffect>
CSpecialEffect* SpecialEffectCreator(HWND hWnd, HBITMAP hBmpBkgnd)
{
	return static_cast<CSpecialEffect*>(new SpecialEffect(hWnd, hBmpBkgnd));
}

class EffectAutoRegister
{
public:
	EffectAutoRegister(
		EffectRegistry::CreateEffectFunc creator,
		const char* name)
	{
		EffectRegistry::RegisterEffect(creator, name);
	}
};

#define REGISTER_EFFECT(class_name) \
	static EffectAutoRegister class_name ## _register( \
		SpecialEffectCreator<class_name>, \
		#class_name)

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

class EffectAutoRegisterForTest
{
public:
	EffectAutoRegisterForTest(
		EffectRegistry::CreateEffectFunc creator,
		const char* name,
		const char* date)
	{
		EffectRegistry::RegisterEffectForTest(creator, name, date);
	}
};

#define REGISTER_EFFECT_FOR_TEST(class_name, date) \
	static EffectAutoRegisterForTest class_name ## _register_for_test( \
		SpecialEffectCreator<class_name>, \
		#class_name, \
		date)

#endif // _EFFECT_REGISTER_H__INCLUDED__2008_05_17_
