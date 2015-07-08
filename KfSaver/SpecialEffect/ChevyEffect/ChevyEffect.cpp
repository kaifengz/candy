// ChevyEffect.cpp
//

#include "stdafx.h"
#include "ChevyEffect.h"
#include "ChevyScene.h"
#include "EffectRegister.h"

REGISTER_EFFECT(CChevyEffect);

/*****************************************************************************\
*
*	ģ����������򣨼�ģ�� 2006-09-03����
*	1.	���е�ʵ�壨�����˻����򣩾�ΪԲ�Σ������Ǹ߶�
*	2.	����ʵ����ĳ�̶��ľ��������ڻ
*	3.	����ʵ�����ͬ�����������໥֮�䷢����ײʱ���϶����غ㶨�ɺ������غ�
*		���ɣ����������һ���̶ȵ�˥�����Ǹ�����ײ����������߽���ײʱ���ƣ���
*		������������ƶ������������������Ϊ�����
*	4.	���л����˴��ڶ�����������һ���ļ��ٶ������ⷽ����٣�����ĳ������ٶ�
*	5.	�����޶�������ͨ��������˵���ײ��ö��ܻ�ı䷽��
*	6.	�������κ�Ħ����
*
*	(2007-06-12)
*	7.	���������֮��Ļ��⣬�Ӷ����⼸�����������Ǵ�ӵ��һ��
*
*	(2011-02-06)
*	8.	��ȥ������֮��Ļ��⣬����ֻ�в��ֻ����˻�׷�������⼸��������Χ�ȴ�����
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
	return m_pScene->Initialize(GetWndWidth(), GetWndHeight());
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
