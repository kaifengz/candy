// GwkBestFirstImpl.cpp: implementation of the CGwkBestFirstImpl class.
//

#include "GwkBestFirstImpl.h"
#include "util.h"

const CGwkBaseImpl::CNodeWrapper* CGwkBestFirstImpl::WrapNode(
		const CGwkNode* pNode) const
{
	CWNode_BestFirst *pBFNode = new CWNode_BestFirst(pNode);
	return static_cast<CNodeWrapper*>(pBFNode);
}

////////////////////////////////////////////////////////////////////////////////
//	CGwkBestFirstImpl::CWNode_BestFirst
////////////////////////////////////////////////////////////////////////////////

CGwkBestFirstImpl::CWNode_BestFirst::CWNode_BestFirst(
		const CGwkNode* pNode) :
	CNodeWrapper(NULL)
{
	this->pNode = dynamic_cast<const CGwkNode_BestFirst*>(pNode);
	ASSERT(this->pNode != NULL);
	this->cost_from_src = 0;
	this->estimated_cost_to_dst = 0;
}

CGwkBestFirstImpl::CWNode_BestFirst::CWNode_BestFirst(
		const CWNode_BestFirst* parent,
		const CGwkNode_BestFirst* pNode,
		cost_t cost_from_src,
		cost_t estimated_cost_to_dst) :
	CNodeWrapper(parent)
{
	this->pNode = dynamic_cast<const CGwkNode_BestFirst*>(pNode);
	ASSERT(this->pNode != NULL);
	this->cost_from_src = cost_from_src;
	this->estimated_cost_to_dst = estimated_cost_to_dst;
}

CGwkBestFirstImpl::CWNode_BestFirst::~CWNode_BestFirst()
{
	delete const_cast<CGwkNode_BestFirst*>(this->pNode);
}

bool CGwkBestFirstImpl::CWNode_BestFirst::UniqueLess(
		const CNodeWrapper &another) const
{
	const CWNode_BestFirst *pAnother =
		dynamic_cast<const CWNode_BestFirst*>(&another);
	ASSERT(pAnother != NULL);

	return this->pNode->Compare(*(pAnother->pNode)) < 0;
}

bool CGwkBestFirstImpl::CWNode_BestFirst::ScoreLess(
		const CNodeWrapper &another) const
{
	const CWNode_BestFirst *pAnother =
		dynamic_cast<const CWNode_BestFirst*>(&another);
	ASSERT(pAnother != NULL);

	// nodes have larger cost have less score
	return (this->cost_from_src + this->estimated_cost_to_dst) >
		(pAnother->cost_from_src + pAnother->estimated_cost_to_dst);
}

bool CGwkBestFirstImpl::CWNode_BestFirst::UniqueEqual(
		const CNodeWrapper &another) const
{
	const CWNode_BestFirst *pAnother =
		dynamic_cast<const CWNode_BestFirst*>(&another);
	ASSERT(pAnother != NULL);

	return this->pNode->Compare(*(pAnother->pNode)) == 0;
}

const CGwkNode* CGwkBestFirstImpl::CWNode_BestFirst::GetNode() const
{
	return static_cast<const CGwkNode*>(this->pNode);
}

bool CGwkBestFirstImpl::CWNode_BestFirst::LevelDown(
		const vector<const CGwkNode*> &vDst,
		vector<const CNodeWrapper*> &v) const
{
	ASSERT(v.empty());

	typedef const vector<const CGwkNode*> VGwkNode;
	for(VGwkNode::const_iterator iter=vDst.begin(); iter!=vDst.end(); ++iter)
		ASSERT(NULL != dynamic_cast<const CGwkNode_BestFirst*>(*iter));
	typedef const vector<const CGwkNode_BestFirst*> VGwkNode_BestFirst;
	const VGwkNode_BestFirst* pVDst =
		reinterpret_cast<VGwkNode_BestFirst*>(&vDst);

	typedef vector<pair<const CGwkNode_BestFirst*, cost_t> > pairs_t;

	pairs_t pairs;
	if(! this->pNode->LevelDown(pairs))
		return false;

	for(pairs_t::const_iterator it=pairs.begin(); it!=pairs.end(); ++it)
	{
		const CGwkNode_BestFirst *pNode = (it->first);
		cost_t cost_from_src = this->cost_from_src + it->second;
		cost_t estimated_cost_to_dst = 0;
		VERIFY(pNode->EstimateCost(*pVDst, estimated_cost_to_dst));

		CWNode_BestFirst *pBFNode = new CWNode_BestFirst(
			this, pNode, cost_from_src, estimated_cost_to_dst);
		if(pBFNode == NULL)
			goto release_all_and_fail;
		v.push_back(static_cast<const CNodeWrapper*>(pBFNode));
	}

	pairs.clear();
	return true;

release_all_and_fail:
	v.clear();
	for(pairs_t::iterator iter=pairs.begin(); iter!=pairs.end(); iter++)
	{
		const CGwkNode_BestFirst *pNode =
			static_cast<const CGwkNode_BestFirst *>(iter->first);
		ASSERT(pNode != NULL);
		delete const_cast<CGwkNode_BestFirst*>(pNode);
	}
	pairs.clear();
	return false;
}
