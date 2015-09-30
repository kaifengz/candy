// BlockEffect.cpp
//

#include "stdafx.h"
#include "BlockEffect.h"
#include "BlockScene.h"
#include "EffectRegister.h"

REGISTER_EFFECT(CBlockEffect);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlockEffect::CBlockEffect(HWND hWnd, HBITMAP hBmpBkgnd) :
	CSpecialEffect(hWnd, hBmpBkgnd, ES_FILL_BACKGROUND)
{
	m_pScene = new NBlock::CBlockScene();
}

CBlockEffect::~CBlockEffect()
{
	if(m_pScene != NULL)
		delete m_pScene;
}

const char* CBlockEffect::GetEffectName() const
{
	return "BlockEffect";
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

BOOL CBlockEffect::Initialize(HDC hDC, BOOL)
{
	return m_pScene->Initialize(hDC);
}

BOOL CBlockEffect::OnTimer(HDC hDC)
{
	if(! m_pScene->OnTimer())
		return FALSE;
	if(! m_pScene->Draw(hDC, GetClientArea()))
		return FALSE;
	return TRUE;
}
