// PurlerEffect.cpp
//

#include "stdafx.h"
#include "PurlerEffect.h"
#include "PurlerScene.h"
#include "EffectRegister.h"

// REGISTER_EFFECT(CPurlerEffect);
REGISTER_EFFECT_FOR_TEST(CPurlerEffect, "2008-05-17");

/*****************************************************************************\
*
*	ʵ��׹��ģ�⣨2006-10-01����
*	1.	��������Ϊһ��ֱ�Ķ�ά�ռ䣬�ö�ά�ռ�����������໥ƽ�е���ֱֱ��֮
*       ��
*	2.	����ϸ���������Ϊ�㣩�Ӹ߿�����׹�䣬׹����������������ʵ㣨��СΪ
*       �㣬���������������ײ��������Ȼ����һ���ķ������ٶȡ���ת����
*       �ȼ���׹��
*	3.	����ϸ�����п����뷢�������ı߽���ײ������߽����������󣩴Ӷ��ı�
*       �˶�������ٶ�
*	4.	���е���ײ�����������غ㶨�������غ㶨���Լ��Ƕ����غ㶨��ÿ����
*       ײ����һ���ٷֱȵĶ���ת��Ϊ���ܣ������ܴ���һ������ģ�
*	5.	�������κ�Ħ����
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
