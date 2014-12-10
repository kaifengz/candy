
#ifndef _GWK_BEST_FIRST_IMPL_H__INCLUDED__2007_04_08_
#define _GWK_BEST_FIRST_IMPL_H__INCLUDED__2007_04_08_

#include "GwkBestFirst.h"
#include "GwkBaseImpl.h"

class CGwkBestFirstImpl: public CGwkBaseImpl
{
protected:
	class CWNode_BestFirst: public CGwkBaseImpl::CNodeWrapper
	{
	public:
		typedef CGwkNode_BestFirst::cost_t cost_t;

	public:
		CWNode_BestFirst(const CGwkNode* pNode);
		virtual ~CWNode_BestFirst();
	protected:
		CWNode_BestFirst(const CWNode_BestFirst* parent,
						 const CGwkNode_BestFirst* pNode,
						 cost_t cost_from_src,
						 cost_t estimated_cost_to_dst);

	public:
		virtual bool UniqueLess(const CNodeWrapper &) const;
		virtual bool UniqueEqual(const CNodeWrapper &) const;
		virtual bool ScoreLess(const CNodeWrapper &) const;
		virtual const CGwkNode* GetNode() const;
		virtual bool LevelDown(const vector<const CGwkNode*> &vDst,
							   vector<const CNodeWrapper*> &) const;

	protected:
		const CGwkNode_BestFirst* pNode;
		cost_t cost_from_src;
		cost_t estimated_cost_to_dst;
	};

	virtual const CNodeWrapper* WrapNode(const CGwkNode*) const;
};

#endif // _GWK_BEST_FIRST_IMPL_H__INCLUDED__2007_04_08_
