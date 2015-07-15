// PurlerEffect.cpp
//

#include "stdafx.h"
#include "PurlerEffect.h"
#include "PurlerScene.h"
#include "EffectRegister.h"

// REGISTER_EFFECT(CPurlerEffect);
REGISTER_EFFECT_FOR_TEST(CPurlerEffect, "2014-12-13");

/*****************************************************************************\
*
*	实体坠落模拟（2006-10-01）：
*	1.	发生场景为一竖直的二维空间，该二维空间局限在两条相互平行的竖直直线之
*       间
*	2.	刚性细棒（即宽度为零）从高空自由坠落，坠落过程中与拦腰的质点（大小为
*       零，假设质量无穷大）碰撞并被弹起，然后以一定的方向、线速度、旋转角速
*       度继续坠落
*	3.	刚性细棒亦有可能与发生场景的边界碰撞（假设边界的质量无穷大）从而改变
*       运动方向和速度
*	4.	所有的碰撞均满足能量守恒定理、动量守恒定理以及角动量守恒定理，每次碰
*       撞均有一定百分比的动能转换为内能（即动能存在一定的损耗）
*	5.	不考虑任何摩擦力
*
\*****************************************************************************/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPurlerEffect::CPurlerEffect(HWND hWnd, HBITMAP hBmpBkgnd) :
    CSpecialEffect(hWnd, hBmpBkgnd, ES_FILL_BACKGROUND)
{
	m_pScene = new NPurler::CPurlerScene();
}

CPurlerEffect::~CPurlerEffect()
{
	if(m_pScene != NULL)
		delete m_pScene;
}

const char* CPurlerEffect::GetEffectName() const
{
    return "PurlerEffect";
}

//////////////////////////////////////////////////////////////////////////
//  
//////////////////////////////////////////////////////////////////////////

BOOL CPurlerEffect::Initialize(HDC hDC, BOOL)
{
	return m_pScene->Initialize(GetWndWidth(), GetWndHeight());
}

BOOL CPurlerEffect::OnTimer(HDC hDC)
{
	if(GetFrameCount() != 0)
	{
		if(!m_pScene->Act())
			return FALSE;
	}

	if(!m_pScene->Draw(hDC))
		return FALSE;

	return !m_pScene->IsEnd();
}

DWORD CPurlerEffect::GetTimeslice() const
{
	return m_pScene->GetTimeslice();
}
