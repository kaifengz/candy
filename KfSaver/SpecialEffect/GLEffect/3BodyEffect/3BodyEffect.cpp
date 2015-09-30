// 3BodyEffect.cpp

#include "StdAfx.h"
#include "3BodyEffect.h"
#include "3BodyScene.h"
#include "EffectRegister.h"

REGISTER_EFFECT(C3BodyEffect);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3BodyEffect::C3BodyEffect(HWND hWnd, HBITMAP hBmpBkgnd) :
	CSpecialEffect(hWnd, hBmpBkgnd, ES_FILL_BACKGROUND)
{
	m_pScene = new N3Body::C3BodyScene();
	ASSERT(m_pScene != NULL);
}

C3BodyEffect::~C3BodyEffect()
{
	if(m_pScene != NULL)
		delete m_pScene;
}

const char* C3BodyEffect::GetEffectName() const
{
	return "3BodyEffect";
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

BOOL C3BodyEffect::Initialize(HDC, BOOL)
{
	if(m_pScene->Initialize(GetClientArea()))
		return TRUE;
	else
		return FALSE;
}

BOOL C3BodyEffect::OnTimer(HDC hDC)
{
	if(GetFrameCount() != 0)
	{
		if(!m_pScene->Act())
			return FALSE;
	}

	if(!m_pScene->Draw(hDC))
		return FALSE;

	if(m_pScene->SceneEnd())
		return FALSE;

	return GetElapsedTime() < m_pScene->GetMaxTime();
}

DWORD C3BodyEffect::GetTimeslice() const
{
	return (DWORD)(m_pScene->GetTimeslice());
}
