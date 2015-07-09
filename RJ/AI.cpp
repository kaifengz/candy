
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

	{	// 找出所以可以打出的牌
		int iCard;
		int iSuit;
		int i;

		for(i=0; i<m_PlayerCards[m_iCurrentTurn].size(); i++)
		{
			iCard = m_PlayerCards[m_iCurrentTurn][i].iCard;

			if(VALUE(iCard) == CENTER_CARD) // 是中间的牌（即是七）
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

	if(legal_out.size() == 1) // 只有一张可以打出的
		return legal_out[0];

	// 由于中级的策略对自己更有利，故先考虑中级策略再考虑高级策略
	if(index<0 && (m_ai==AI_HARD || m_ai==AI_NORMAL))
	{
		index = AINormal(legal_out);
	}

	// 高级策略是对中级策略的补充，由于中级策略能够实现的时候不多，
	// 在更多的时候，需要高级策略来使自己的形势变得更有利
	if(index<0 && m_ai==AI_HARD)
	{
		index = AIHard(legal_out);
	}

	// 在别无选择的情况下，只有随机打出一张牌了
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
	// 尽可能的使自己不减少可以打出的牌的数目
	// 1. 如果拥有同花色的678，则打出7可以使自己增加一张可打出牌
	// 2. 如果拥有连续的牌（如234），并且该序列的头（尾）可以打
	//    出，则打出该头（尾）不会减少自己可打出牌的数目
	// 3. 如果自己手中的A或K可以打出，则打出它起码不会增加敌手的
	//    可打出牌数目

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
	// 使自己的牌更容易打出

	// 假如自己的某一张牌不打出的话则自己将有k张牌永远无法打出（例
	// 如自己拥有146，则如果不打出6，1和4将无法打出），则称该派的
	// 权重为k（此处6的权重为2）

	// 七的权重应该等于自己拥有的同种花色牌的数目

	// 1. 统计所有可打出牌的权重，打出权重最大的牌
	// 2. 如果权重最大者又多张，选择一张对于敌方权重最小的打出

	const int n = legal_out.size();
	const ZPlayerCards &pc = m_PlayerCards[m_iCurrentTurn];

	vector<int> weight;		// 对自权重
	vector<int> weight_v;	// 对敌权重
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
