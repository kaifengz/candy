// BumperEffect.cpp
//

#include "stdafx.h"
#include "BumperEffect.h"
#include "BumperScene.h"
#include "EffectRegister.h"

REGISTER_EFFECT_FOR_TEST(CBumperEffect, "2011-01-28");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBumperEffect::CBumperEffect(HWND hWnd, HBITMAP hBmpBkgnd) :
	CSpecialEffect(hWnd, hBmpBkgnd, ES_FILL_BACKGROUND)
{
	m_pScene = new NBumper::CBumperScene();
}

CBumperEffect::~CBumperEffect()
{
	if(m_pScene != NULL)
		delete m_pScene;
}

const char* CBumperEffect::GetEffectName() const
{
	return "BumperEffect";
}

//////////////////////////////////////////////////////////////////////////
//  
//////////////////////////////////////////////////////////////////////////

BOOL CBumperEffect::Initialize(HDC hDC, BOOL)
{
	return m_pScene->Initialize(GetWndWidth(), GetWndHeight());
}

BOOL CBumperEffect::OnTimer(HDC hDC)
{
	if(GetFrameCount() != 0)
	{
		if(!m_pScene->Act())
			return FALSE;
	}

	if(!m_pScene->Draw(hDC))
		return FALSE;

	return GetElapsedTime() < m_pScene->GetMaxTime();
}

DWORD CBumperEffect::GetTimeslice() const
{
	return m_pScene->GetTimeslice();
}
