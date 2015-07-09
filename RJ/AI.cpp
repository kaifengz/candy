
#include "stdafx.h"
#include "RJ.h"
#include "ChildView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifndef _DEBUG
#	ifdef ASSERT
#		undef ASSERT
#	endif

#	define ASSERT(f)	if(f);else ErrorReport(__FILE__, __LINE__)
#endif


int CChildView::ComputerThink()
{
	int index = AIFirstOut();

	if(index >= 0)
		return index;

	vector<int> legal_out;

	{	// �ҳ����Կ��Դ������
		int iCard;
		int iSuit;
		int i;

		for(i=0; i<m_PlayerCards[m_iCurrentTurn].size(); i++)
		{
			iCard = m_PlayerCards[m_iCurrentTurn][i].iCard;

			if(VALUE(iCard) == CENTER_CARD) // ���м���ƣ������ߣ�
				legal_out.push_back(i);
			else
			{
				iSuit = SUIT(iCard);
				if(m_CardList[iSuit].first >= 0 && 
				   (iCard == m_CardList[iSuit].first-1 ||
					iCard == m_CardList[iSuit].last+1 ) )
				{
					legal_out.push_back(i);
				}
			}
		}

		ASSERT(legal_out.size() > 0);
	}

	if(legal_out.size() == 1) // ֻ��һ�ſ��Դ����
		return legal_out[0];

	// �����м��Ĳ��Զ��Լ������������ȿ����м������ٿ��Ǹ߼�����
	if(index<0 && (m_ai==AI_HARD || m_ai==AI_NORMAL))
	{
		index = AINormal(legal_out);
	}

	// �߼������Ƕ��м����ԵĲ��䣬�����м������ܹ�ʵ�ֵ�ʱ�򲻶࣬
	// �ڸ����ʱ����Ҫ�߼�������ʹ�Լ������Ʊ�ø�����
	if(index<0 && m_ai==AI_HARD)
	{
		index = AIHard(legal_out);
	}

	// �ڱ���ѡ�������£�ֻ��������һ������
	if(index < 0)
	{
		index = AIEasy(legal_out);
	}

	ASSERT(index >= 0);

	return index;
}

int CChildView::AIFirstOut()
{
	int i;

	for(i=m_PlayerCards[m_iCurrentTurn].size()-1; i>=0; i--)
	{
		if(m_PlayerCards[m_iCurrentTurn][i].iCard == FIRST_OUT_CARD)
			break;
	}

	return i;
}

int CChildView::AIEasy(const vector<int> &legal_out)
{
	return legal_out[ rand()%legal_out.size() ];
}

int CChildView::AINormal(const vector<int> &legal_out)
{
	// �����ܵ�ʹ�Լ������ٿ��Դ�����Ƶ���Ŀ
	// 1. ���ӵ��ͬ��ɫ��678������7����ʹ�Լ�����һ�ſɴ����
	// 2. ���ӵ���������ƣ���234�������Ҹ����е�ͷ��β�����Դ�
	//    ����������ͷ��β����������Լ��ɴ���Ƶ���Ŀ
	// 3. ����Լ����е�A��K���Դ�������������벻�����ӵ��ֵ�
	//    �ɴ������Ŀ

	const int n = legal_out.size();
	const ZPlayerCards &pc = m_PlayerCards[m_iCurrentTurn];
	vector<int> policy_out;
	int iCard;
	int index;
	int i;

	// 1
	policy_out.clear();
	for(i=0; i<n; i++)
	{
		index = legal_out[i];
		iCard = pc[index].iCard;

		if (VALUE(iCard) == CENTER_CARD &&
			index > 0 &&
			pc[index-1].iCard == iCard-1 &&
			index < pc.size()-1 &&
			pc[index+1].iCard == iCard+1 )
		{
			policy_out.push_back(index);
		}
	}
	if(policy_out.size() > 0)
	{
		return policy_out[ rand()%policy_out.size() ];
	}

	// 2
	policy_out.clear();
	for(i=0; i<n; i++)
	{
		index = legal_out[i];
		iCard = pc[index].iCard;

		if (CanSendOut(iCard) &&
			(index > 0 &&
			 VALUE(pc[index-1].iCard) == VALUE(iCard)-1 &&
			 SUIT(pc[index-1].iCard) == SUIT(iCard) ) ||
			(index < pc.size()-1 &&
			 VALUE(pc[index+1].iCard) == VALUE(iCard)+1 &&
			 SUIT(pc[index+1].iCard) == SUIT(iCard) ) )
		{
			policy_out.push_back(index);
		}
	}
	if(policy_out.size() > 0)
	{
		return policy_out[ rand()%policy_out.size() ];
	}

	// 3
	policy_out.clear();
	for(i=0; i<n; i++)
	{
		index = legal_out[i];
		iCard = pc[index].iCard;

		if (CanSendOut(iCard) &&
			(VALUE(iCard) == VALUE_A ||
			 VALUE(iCard) == VALUE_K) )
		{
			policy_out.push_back(index);
		}
	}
	if(policy_out.size() > 0)
	{
		return policy_out[ rand()%policy_out.size() ];
	}

	return -1;
}

int CChildView::AIHard(const vector<int> &legal_out)
{
	// ʹ�Լ����Ƹ����״��

	// �����Լ���ĳһ���Ʋ�����Ļ����Լ�����k������Զ�޷��������
	// ���Լ�ӵ��146������������6��1��4���޷����������Ƹ��ɵ�
	// Ȩ��Ϊk���˴�6��Ȩ��Ϊ2��

	// �ߵ�Ȩ��Ӧ�õ����Լ�ӵ�е�ͬ�ֻ�ɫ�Ƶ���Ŀ

	// 1. ͳ�����пɴ���Ƶ�Ȩ�أ����Ȩ��������
	// 2. ���Ȩ��������ֶ��ţ�ѡ��һ�Ŷ��ڵз�Ȩ����С�Ĵ��

	const int n = legal_out.size();
	const ZPlayerCards &pc = m_PlayerCards[m_iCurrentTurn];

	vector<int> weight;		// ����Ȩ��
	vector<int> weight_v;	// �Ե�Ȩ��
	weight.resize( n );
	weight_v.resize( n );

	int iCard, iSuit, iValue;
	int i, j;
	int max_weight, min_weight_v;
	vector<int> policy_out;

	// sum weight
	for(i=0; i<n; i++)
	{
		iCard = pc[legal_out[i]].iCard;
		iSuit = SUIT(iCard);
		iValue = VALUE(iCard);

		weight[i] = 0;

		for(j=0; j<pc.size(); j++)
		{
			if(SUIT(pc[j].iCard) == iSuit)
			{
				if ((iValue <= CENTER_CARD && VALUE(pc[j].iCard)<iValue) ||
					(iValue >= CENTER_CARD && VALUE(pc[j].iCard)>iValue) )
				{
					weight[i]++;
				}
			}
		}

		weight_v[i] = 0;
		if(iValue <= CENTER_CARD)
			weight_v[i] += iValue;
		if(iValue >= CENTER_CARD)
			weight_v[i] += (SUIT_EACH - 1 - iValue);
		weight_v[i] -= weight[i];
	}

	// check the max weight
	max_weight = weight[0];
	min_weight_v = weight_v[0];
	for(i=1; i<n; i++)
	{
		if ((weight[i] > max_weight) ||
			(weight[i] == max_weight && weight_v[i] < min_weight_v))
		{
			max_weight = weight[i];
			min_weight_v = weight_v[i];
		}
	}

	// get the index with the max weight
	for(i=0; i<n; i++)
	{
		if (weight[i] == max_weight &&
			weight_v[i] == min_weight_v)
		{
			policy_out.push_back( legal_out[i] );
		}
	}

	// select random one to send out
	return policy_out[ rand()%policy_out.size() ];
}
