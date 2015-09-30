// ChevyEffect.cpp
//

#include "stdafx.h"
#include "ChevyEffect.h"
#include "ChevyScene.h"
#include "EffectRegister.h"

REGISTER_EFFECT(CChevyEffect);

/*****************************************************************************\
*
*	模拟机器人足球（简化模型 2006-09-03）：
*	1.	所有的实体（机器人或足球）均为圆形，不考虑高度
*	2.	所有实体在某固定的矩形区域内活动
*	3.	所有实体具有同样的质量，相互之间发生碰撞时符合动量守恒定律和能量守恒
*		定律，动能则存在一定程度的衰减（非刚性碰撞）；与区域边界碰撞时相似，但
*		不考虑区域的移动（即假设区域的质量为无穷大）
*	4.	所有机器人存在动力，可以以一定的加速度沿任意方向加速，存在某最大线速度
*	5.	足球无动力，可通过与机器人的碰撞获得动能或改变方向
*	6.	不考虑任何摩擦力
*
*	(2007-06-12)
*	7.	加入机器人之间的互斥，从而避免几个机器人总是簇拥在一起
*
*	(2011-02-06)
*	8.	除去机器人之间的互斥，现在只有部分机器人会追逐球，另外几个将在周围等待机会
*
\*****************************************************************************/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChevyEffect::CChevyEffect(HWND hWnd, HBITMAP hBmpBkgnd) :
	CSpecialEffect(hWnd, hBmpBkgnd, ES_FILL_BACKGROUND)
{
	m_pScene = new NChevy::CChevyScene();
}

CChevyEffect::~CChevyEffect()
{
	if(m_pScene != NULL)
		delete m_pScene;
}

const char* CChevyEffect::GetEffectName() const
{
	return "ChevyEffect";
}

//////////////////////////////////////////////////////////////////////////
//  
//////////////////////////////////////////////////////////////////////////

BOOL CChevyEffect::Initialize(HDC hDC, BOOL)
{
	return m_pScene->Initialize(GetClientArea());
}

BOOL CChevyEffect::OnTimer(HDC hDC)
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

DWORD CChevyEffect::GetTimeslice() const
{
	return m_pScene->GetTimeslice();
}
