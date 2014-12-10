// GwkBaseImpl.cpp: implementation of the CGwkBaseImpl class.
//

#include "GwkBase.h"
#include "GwkBaseImpl.h"
#include "util.h"

#include <time.h>

////////////////////////////////////////////////////////////////////////////////
//	CGwkBaseImpl
////////////////////////////////////////////////////////////////////////////////

CGwkBaseImpl::CGwkBaseImpl()
{
}

CGwkBaseImpl::~CGwkBaseImpl()
{
}

HGWK_RESULT CGwkBaseImpl::Search(const CGwkNode* pSrc,
								 const std::vector<const CGwkNode*> vDst,
								 const std::vector<const CGwkNode*> vObstacle,
								 GwkSearchStrategy strategy,
								 unsigned int strategy_param)
{
	const CGwkNode* pClonedSrc = pSrc->Clone();
	ASSERT(pClonedSrc != NULL);
	const CNodeWrapper *pWNodeSrc = WrapNode(pClonedSrc);
	ASSERT(pWNodeSrc != NULL);

	NodeSet open_set;
	NodeSet close_set;
	NodeSet goal_set;
	VERIFY(goal_set.FromNodes(this, vDst));
	NodeSet obstacle_set;
	VERIFY(obstacle_set.FromNodes(this, vObstacle));
	ObsoleteNodeSet obsolete_set;

	const CNodeWrapper *pWNodeBest = NULL;
	bool bDstReach = false;

	const clock_t tStart = clock();
	unsigned int nVisitedNode = 0;

	VERIFY(open_set.InsertNode(pWNodeSrc));
	while(!open_set.Empty())
	{
		if(strategy == CGwkBase::GSS_VISITED_NODE_COUNT && nVisitedNode > strategy_param)
			break;
		else if(strategy == CGwkBase::GSS_ELAPSED_TIME && (unsigned int)((clock()-tStart)*1000/CLOCKS_PER_SEC) > strategy_param)
			break;
		++nVisitedNode;

		const CNodeWrapper* pWNodeHighestScore =
			open_set.PopHighestScoreNode();
		ASSERT(pWNodeHighestScore != NULL);
		pWNodeBest = pWNodeHighestScore;

		VERIFY(close_set.InsertNode(pWNodeHighestScore));

		if(goal_set.InSet(pWNodeHighestScore))
		{
			bDstReach = true;
			break;	// Succeed when destination found
		}

		typedef vector<const CNodeWrapper*> WNodes;
		WNodes successor;
		VERIFY(pWNodeHighestScore->LevelDown(vDst, successor));

		for(WNodes::const_iterator iter=successor.begin();
			iter!=successor.end();
			++iter)
		{
			const CNodeWrapper* pSucc = (*iter);
			if(obstacle_set.InSet(pSucc))
			{
				delete const_cast<CNodeWrapper*>(pSucc);
				continue;
			}

			bool bDiscardable = false;
			const CNodeWrapper* pObsolete = NULL;
			if(open_set.InSet(pSucc))
			{
				ASSERT(!close_set.InSet(pSucc));
				if(open_set.HasHigherScore(pSucc))
					VERIFY(NULL != (pObsolete = open_set.PopNode(pSucc)));
				else
					bDiscardable = true;
			}
			else if(close_set.InSet(pSucc))
			{
				if(close_set.HasHigherScore(pSucc))
					VERIFY(NULL != (pObsolete = close_set.PopNode(pSucc)));
				else
					bDiscardable = true;
			}
			if(bDiscardable)
				delete const_cast<CNodeWrapper*>(pSucc);
			else
			{
				VERIFY(open_set.InsertNode(pSucc));
				if(pObsolete != NULL)
					VERIFY(obsolete_set.InsertNode(pObsolete));
			}
		}
	}

	HGWK_RESULT hResult = NULL;
	if(pWNodeBest!=NULL)
	{
		if(strategy == CGwkBase::GSS_REACH_DST)
			ASSERT(goal_set.InSet(pWNodeBest));

		hResult = NewResult();
		ASSERT(hResult != NULL);
		for(const CNodeWrapper* pWNode=pWNodeBest;
			pWNode!=NULL;
			pWNode=pWNode->GetParent())
		{
			ASSERT(close_set.InSet(pWNode) || obsolete_set.InSet(pWNode));
			ASSERT(!close_set.InSet(pWNode) || !obsolete_set.InSet(pWNode));
			if(close_set.InSet(pWNode))
				VERIFY(pWNode == (close_set.PopNode(pWNode)));
			else
				VERIFY(pWNode == (obsolete_set.PopNode(pWNode)));
			VERIFY(PrependNodeToResult(hResult, pWNode));
		}

		ASSERT(GetFirstNode(hResult)->Compare(*pClonedSrc) == 0);
	}

	VERIFY(open_set.ClearAll());
	VERIFY(close_set.ClearAll());
	VERIFY(goal_set.ClearAll());
	VERIFY(obstacle_set.ClearAll());
	VERIFY(obsolete_set.ClearAll());

	// pWNodeSrc and pClonedSrc will be automaticlly deleted by either m_open or m_close
	return hResult;
}

const CGwkNode* CGwkBaseImpl::GetFirstNode(HGWK_RESULT hResult)
{
	ASSERT(hResult != NULL);
	CGwkResult *pResult = reinterpret_cast<CGwkResult*>(hResult);
	return pResult->GetFirstNode();
}

const CGwkNode* CGwkBaseImpl::GetLastNode(HGWK_RESULT hResult)
{
	ASSERT(hResult != NULL);
	CGwkResult *pResult = reinterpret_cast<CGwkResult*>(hResult);
	return pResult->GetLastNode();
}

const CGwkNode* CGwkBaseImpl::GetNextNode(HGWK_RESULT hResult)
{
	ASSERT(hResult != NULL);
	CGwkResult *pResult = reinterpret_cast<CGwkResult*>(hResult);
	return pResult->GetNextNode();
}

const CGwkNode* CGwkBaseImpl::GetPrevNode(HGWK_RESULT hResult)
{
	ASSERT(hResult != NULL);
	CGwkResult *pResult = reinterpret_cast<CGwkResult*>(hResult);
	return pResult->GetPrevNode();
}

void CGwkBaseImpl::ReleaseResult(HGWK_RESULT &hResult)
{
	if(hResult != NULL)
	{
		CGwkResult *pResult = reinterpret_cast<CGwkResult*>(hResult);
		delete pResult;
		hResult = NULL;
	}
}

HGWK_RESULT CGwkBaseImpl::NewResult()
{
	CGwkResult *pResult = new CGwkResult();
	return static_cast<HGWK_RESULT>(pResult);
}

bool CGwkBaseImpl::AppendNodeToResult(HGWK_RESULT hResult, const CNodeWrapper* pNode)
{
	ASSERT(hResult != NULL);
	CGwkResult *pResult = reinterpret_cast<CGwkResult*>(hResult);
	return pResult->push_back(pNode);
}

bool CGwkBaseImpl::PrependNodeToResult(HGWK_RESULT hResult, const CNodeWrapper* pNode)
{
	ASSERT(hResult != NULL);
	CGwkResult *pResult = reinterpret_cast<CGwkResult*>(hResult);
	return pResult->push_front(pNode);
}

////////////////////////////////////////////////////////////////////////////////
//	CGwkBaseImpl::CNodeWrapper
////////////////////////////////////////////////////////////////////////////////

CGwkBaseImpl::CNodeWrapper::CNodeWrapper(const CNodeWrapper* parent)
{
	this->parent = parent;
}

CGwkBaseImpl::CNodeWrapper::~CNodeWrapper()
{
}

const CGwkBaseImpl::CNodeWrapper* CGwkBaseImpl::CNodeWrapper::GetParent() const
{
	return this->parent;
}

bool CGwkBaseImpl::CNodeWrapper::UniqueEqual(const CNodeWrapper &another) const
{
	return !UniqueLess(another) && !another.UniqueLess(*this);
}

////////////////////////////////////////////////////////////////////////////////
//	CGwkBaseImpl::CGwnResult
////////////////////////////////////////////////////////////////////////////////

CGwkBaseImpl::CGwkResult::CGwkResult()
{
	freezed = false;
	have_got_the_first = false;
}

CGwkBaseImpl::CGwkResult::~CGwkResult()
{
	CGwkBaseImpl::ReleaseWNodes(path);
}

const CGwkNode* CGwkBaseImpl::CGwkResult::GetFirstNode()
{
	if(this->path.empty())
		return NULL;

	freezed = true;
	have_got_the_first = true;
	cur_visited_node = path.begin();
	return (*cur_visited_node)->GetNode();
}

const CGwkNode* CGwkBaseImpl::CGwkResult::GetLastNode()
{
	if(this->path.empty())
		return NULL;

	freezed = true;
	have_got_the_first = true;
	cur_visited_node = path.end();
	--cur_visited_node;
	return (*cur_visited_node)->GetNode();
}

const CGwkNode* CGwkBaseImpl::CGwkResult::GetNextNode()
{
	ASSERT(have_got_the_first);

	if(cur_visited_node == path.end())
		return NULL;
	if(++cur_visited_node == path.end())
		return NULL;

	return (*cur_visited_node)->GetNode();
}

const CGwkNode* CGwkBaseImpl::CGwkResult::GetPrevNode()
{
	ASSERT(have_got_the_first);

	if(cur_visited_node == path.begin())
		return NULL;
	if(--cur_visited_node == path.begin())
		return NULL;

	return (*cur_visited_node)->GetNode();
}

bool CGwkBaseImpl::CGwkResult::push_front(const CNodeWrapper*pWNode)
{
	ASSERT(!freezed);

	this->path.push_front(pWNode);
	return true;
}

bool CGwkBaseImpl::CGwkResult::push_back(const CNodeWrapper*pWNode)
{
	ASSERT(!freezed);

	this->path.push_front(pWNode);
	return true;
}

////////////////////////////////////////////////////////////////////////////////
//	CGwkBaseImpl::NodeSet
////////////////////////////////////////////////////////////////////////////////

bool CGwkBaseImpl::NodeSet::UniqueLess::operator ()(
		const Node& a, const Node& b) const
{
	const CNodeWrapper* pNodeA = a;
	const CNodeWrapper* pNodeB = b;
	return pNodeA->UniqueLess(*pNodeB);
}

bool CGwkBaseImpl::NodeSet::ScoreLess::operator ()(
		const Node& a, const Node& b) const
{
	const CNodeWrapper* pNodeA = a;
	const CNodeWrapper* pNodeB = b;
	return pNodeA->ScoreLess(*pNodeB);
}

CGwkBaseImpl::NodeSet::NodeSet()
{
}

CGwkBaseImpl::NodeSet::~NodeSet()
{
	VERIFY(ClearAll());
}

bool CGwkBaseImpl::NodeSet::FromNodes(const CGwkBaseImpl* pGwkImpl,
									  const std::vector<const CGwkNode*> &nodes)
{
	typedef const std::vector<const CGwkNode*> nodes_t;
	for(nodes_t::const_iterator iter=nodes.begin(); iter!=nodes.end(); ++iter)
	{
		const CGwkNode* pClonedNode = (*iter)->Clone();
		ASSERT(pClonedNode != NULL);
		const CGwkBaseImpl::CNodeWrapper* pWNode = pGwkImpl->WrapNode(pClonedNode);
		ASSERT(pWNode != NULL);
		VERIFY(InsertNode(pWNode));
	}

	return true;
}

bool CGwkBaseImpl::NodeSet::Empty() const
{
	ASSERT(m_us.size() == m_ss.size());
	return m_us.empty();
}

bool CGwkBaseImpl::NodeSet::InSet(const CNodeWrapper* pWNode) const
{
	ASSERT(pWNode != NULL);
	ASSERT(m_us.size() == m_ss.size());

	Node node = static_cast<Node>(pWNode);
	UniqueSet::const_iterator iter = m_us.find(node);
	if(iter == m_us.end())
		return false;
	return true;
}

bool CGwkBaseImpl::NodeSet::InsertNode(const CNodeWrapper* pWNode)
{
	ASSERT(pWNode != NULL);
	ASSERT(m_us.size() == m_ss.size());
	ASSERT(!InSet(pWNode));

	Node node = static_cast<Node>(pWNode);
	m_us.insert(node);
	m_ss.insert(node);
	return true;
}

bool CGwkBaseImpl::NodeSet::HasHigherScore(const CNodeWrapper* pWNode) const
{
	ASSERT(pWNode != NULL);
	ASSERT(m_us.size() == m_ss.size());
	ASSERT(InSet(pWNode));

	Node node = static_cast<Node>(pWNode);

	UniqueSet::const_iterator original_node_in_us = m_us.find(node);
	ASSERT(original_node_in_us != m_us.end());

	const CNodeWrapper* original_node =
		static_cast<const CNodeWrapper*>(*original_node_in_us);

	if(original_node->ScoreLess(*pWNode))
		return true;
	return false;
}

const CGwkBaseImpl::CNodeWrapper* CGwkBaseImpl::NodeSet::PopHighestScoreNode()
{
	ASSERT(m_us.size() == m_ss.size());
	ASSERT(! m_ss.empty());

	ScoreSet::iterator end_in_ss = m_ss.end();
	ScoreSet::iterator highest_score_in_ss = --end_in_ss;
	Node node = *highest_score_in_ss;
	UniqueSet::iterator highest_score_in_us = m_us.find(node);
	ASSERT(highest_score_in_us != m_us.end());

	m_us.erase(highest_score_in_us);
	m_ss.erase(highest_score_in_ss);

	const CNodeWrapper *pWNode = static_cast<const CNodeWrapper*>(node);
	ASSERT(pWNode != NULL);
	return pWNode;
}

bool CGwkBaseImpl::NodeSet::ClearAll()
{
	ASSERT(m_us.size() == m_ss.size());

	CGwkBaseImpl::ReleaseWNodes(m_us);
	m_us.clear();
	m_ss.clear();

	return true;
}

const CGwkBaseImpl::CNodeWrapper* CGwkBaseImpl::NodeSet::PopNode(const CNodeWrapper* pWNode)
{
	ASSERT(pWNode != NULL);
	ASSERT(m_us.size() == m_ss.size());
	ASSERT(InSet(pWNode));

	Node node = static_cast<Node>(pWNode);

	UniqueSet::iterator original_node_in_us = m_us.find(node);
	ASSERT(original_node_in_us != m_us.end());

	Node original_node = (*original_node_in_us);

	pair<ScoreSet::iterator, ScoreSet::iterator> equal_range =
		m_ss.equal_range(original_node);

	ScoreSet::iterator original_node_in_ss = m_ss.end();
	for(ScoreSet::iterator iter=equal_range.first; iter!=equal_range.second; ++iter)
	{
		const CNodeWrapper *wnode_in_ss =
			static_cast<const CNodeWrapper*>(*iter);
		const CNodeWrapper *original_wnode =
			static_cast<const CNodeWrapper*>(original_node);
		if( wnode_in_ss->UniqueEqual( *original_wnode ))
		{
			original_node_in_ss = iter;
			break;
		}
	}
	ASSERT(original_node_in_ss != m_ss.end());

	m_us.erase(original_node_in_us);
	m_ss.erase(original_node_in_ss);

	const CNodeWrapper* pWNodeOriginal =
		static_cast<const CNodeWrapper*>(original_node);
	ASSERT(pWNodeOriginal != NULL);
	return pWNodeOriginal;
}

////////////////////////////////////////////////////////////////////////////////
//	CGwkBaseImpl::ObsoleteNodeSet
////////////////////////////////////////////////////////////////////////////////

bool CGwkBaseImpl::ObsoleteNodeSet::UniqueLess::operator ()(
		const Node& a, const Node& b) const
{
	const CNodeWrapper* pNodeA = a;
	const CNodeWrapper* pNodeB = b;
	const void* pA = static_cast<const void*>(pNodeA);
	const void* pB = static_cast<const void*>(pNodeB);
	return pA < pB;
}

CGwkBaseImpl::ObsoleteNodeSet::ObsoleteNodeSet()
{
}

CGwkBaseImpl::ObsoleteNodeSet::~ObsoleteNodeSet()
{
	VERIFY(ClearAll());
}

bool CGwkBaseImpl::ObsoleteNodeSet::InSet(const CNodeWrapper* pWNode) const
{
	ASSERT(pWNode != NULL);

	Node node = static_cast<Node>(pWNode);
	UniqueSet::const_iterator iter = m_us.find(node);
	if(iter == m_us.end())
		return false;
	return true;
}

bool CGwkBaseImpl::ObsoleteNodeSet::InsertNode(const CNodeWrapper* pWNode)
{
	ASSERT(pWNode != NULL);
	ASSERT(!InSet(pWNode));

	Node node = static_cast<Node>(pWNode);
	m_us.insert(node);
	return true;
}

const CGwkBaseImpl::CNodeWrapper* CGwkBaseImpl::ObsoleteNodeSet::PopNode(const CNodeWrapper* pWNode)
{
	ASSERT(pWNode != NULL);
	ASSERT(InSet(pWNode));

	Node node = static_cast<Node>(pWNode);

	UniqueSet::iterator original_node_in_us = m_us.find(node);
	ASSERT(original_node_in_us != m_us.end());

	Node original_node = (*original_node_in_us);
	m_us.erase(original_node_in_us);

	const CNodeWrapper* pWNodeOriginal =
		static_cast<const CNodeWrapper*>(original_node);
	ASSERT(pWNodeOriginal != NULL);
	return pWNodeOriginal;
}

bool CGwkBaseImpl::ObsoleteNodeSet::ClearAll()
{
	CGwkBaseImpl::ReleaseWNodes(m_us);
	m_us.clear();
	return true;
}
