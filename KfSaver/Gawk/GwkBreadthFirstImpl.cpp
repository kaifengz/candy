// GwkBreadthFirstImpl.cpp: implementation of the CGwkBreadthFirstImpl class.
//

#include "GwkBreadthFirstImpl.h"
#include "util.h"

const CGwkBaseImpl::CNodeWrapper* CGwkBreadthFirstImpl::WrapNode(
		const CGwkNode* pNode) const
{
	CWNode_BreadthFirst *pBFNode = new CWNode_BreadthFirst(pNode);
	return static_cast<const CNodeWrapper*>(pBFNode);
}

////////////////////////////////////////////////////////////////////////////////
//	CGwkBreadthFirstImpl::CWNode_BreadthFirst
////////////////////////////////////////////////////////////////////////////////

CGwkBreadthFirstImpl::CWNode_BreadthFirst::CWNode_BreadthFirst(
	const CGwkNode* pNode) :
	CNodeWrapper(NULL)
{
	ASSERT(pNode != NULL);
	this->pNode = pNode;
	this->depth = 0;
}

CGwkBreadthFirstImpl::CWNode_BreadthFirst::CWNode_BreadthFirst(
	const CWNode_BreadthFirst *parent, const CGwkNode *pNode, unsigned depth) :
	CNodeWrapper(parent)
{
	ASSERT(pNode != NULL);
	this->pNode = pNode;
	this->depth = depth;
}

CGwkBreadthFirstImpl::CWNode_BreadthFirst::~CWNode_BreadthFirst()
{
	delete const_cast<CGwkNode*>(this->pNode);
}

bool CGwkBreadthFirstImpl::CWNode_BreadthFirst::UniqueLess(
		const CGwkBaseImpl::CNodeWrapper &another) const
{
	const CWNode_BreadthFirst *pAnother =
		dynamic_cast<const CWNode_BreadthFirst*>(&another);
	ASSERT(pAnother != NULL);

	return this->pNode->Compare(*(pAnother->pNode)) < 0;
}

bool CGwkBreadthFirstImpl::CWNode_BreadthFirst::ScoreLess(
		const CGwkBaseImpl::CNodeWrapper &another) const
{
	const CWNode_BreadthFirst *pAnother =
		dynamic_cast<const CWNode_BreadthFirst*>(&another);
	ASSERT(pAnother != NULL);

	// nodes have larger depth have less score
	return (this->depth) > (pAnother->depth);
}

bool CGwkBreadthFirstImpl::CWNode_BreadthFirst::UniqueEqual(
		const CGwkBaseImpl::CNodeWrapper &another) const
{
	const CWNode_BreadthFirst *pAnother =
		dynamic_cast<const CWNode_BreadthFirst*>(&another);
	ASSERT(pAnother != NULL);

	return this->pNode->Compare(*(pAnother->pNode)) == 0;
}

const CGwkNode* CGwkBreadthFirstImpl::CWNode_BreadthFirst::GetNode() const
{
	return this->pNode;
}

bool CGwkBreadthFirstImpl::CWNode_BreadthFirst::LevelDown(
		const vector<const CGwkNode*> & /* vDst */,
		vector<const CNodeWrapper*> &v) const
{
	ASSERT(v.empty());

	typedef vector<const CGwkNode*> Nodes;

	Nodes nodes;
	if(! this->pNode->LevelDown(nodes))
		return false;

	for(Nodes::const_iterator it=nodes.begin(); it!=nodes.end(); ++it)
	{
		CWNode_BreadthFirst *pBFNode =
			new CWNode_BreadthFirst(this, *it, this->depth+1);
		if(pBFNode == NULL)
			goto release_all_and_fail;
		v.push_back(pBFNode);
	}

	return true;

release_all_and_fail:
	v.clear();
	CGwkBaseImpl::ReleaseNodes(nodes);
	return false;
}
