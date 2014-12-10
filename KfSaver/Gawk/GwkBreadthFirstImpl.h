
#ifndef _GWK_BREADTH_FIRST_IMPL_H__INCLUDED__2007_04_06_
#define _GWK_BREADTH_FIRST_IMPL_H__INCLUDED__2007_04_06_

#include "GwkBreadthFirst.h"
#include "GwkBaseImpl.h"

class CGwkBreadthFirstImpl: public CGwkBaseImpl
{
protected:
	class CWNode_BreadthFirst: public CGwkBaseImpl::CNodeWrapper
	{
	public:
		CWNode_BreadthFirst(const CGwkNode* pNode);
		virtual ~CWNode_BreadthFirst();
	protected:
		CWNode_BreadthFirst(const CWNode_BreadthFirst *parent,
							const CGwkNode *pNode,
							unsigned depth);

	public:
		virtual bool UniqueLess(const CNodeWrapper &) const;
		virtual bool UniqueEqual(const CNodeWrapper &) const;
		virtual bool ScoreLess(const CNodeWrapper &) const;
		virtual const CGwkNode* GetNode() const;
		virtual bool LevelDown(const vector<const CGwkNode*> &vDst,
							   vector<const CNodeWrapper*> &) const;

	protected:
		const CGwkNode *pNode;
		unsigned int depth;
	};

	virtual const CNodeWrapper* WrapNode(const CGwkNode*) const;
};

#endif // _GWK_BREADTH_FIRST_IMPL_H__INCLUDED__2007_04_06_
