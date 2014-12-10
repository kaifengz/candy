// EffectFactory.cpp: implementation of the CEffectFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectFactory.h"
#include "EffectRegister.h"

CSpecialEffect* CEffectFactory::CreateEffect(
		HWND hWnd,
		HBITMAP hBmpBkgnd)
{
	srand(GetTickCount());

	EffectRegistry::CreateEffectFunc creator = NULL;
	CSpecialEffect *pEffect = NULL;

#ifdef DEBUG_MODE
	creator = EffectRegistry::GetCreatorForTest();
	if (creator != NULL)
	{
		pEffect = creator(hWnd, hBmpBkgnd);
		if(pEffect != NULL)
			LOG("Create %s for special test", pEffect->GetEffectName());
		return pEffect;
	}
#endif

	static const EffectRegistry::EffectCreators& creators =
		EffectRegistry::GetEffectCreators();
	static std::vector<unsigned int> weight(creators.size());

	unsigned int iEffect = RandomSelect(weight);
	ASSERT(iEffect>=0 && iEffect<weight.size());

	creator = creators[iEffect];
	ASSERT(creator != NULL);

	pEffect = creator(hWnd, hBmpBkgnd);
	if(pEffect != NULL)
		LOG("Create %s", pEffect->GetEffectName());
	return pEffect;
}

void CEffectFactory::DeleteEffect(CSpecialEffect *&pEffect)
{
	if(pEffect != NULL)
	{
		LOG("Delete %s", pEffect->GetEffectName());

		delete pEffect;
		pEffect = NULL;
	}
}
