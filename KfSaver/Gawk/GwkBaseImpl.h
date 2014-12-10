
#ifndef _GWK_BASE_IMPL_H__INCLUDED__2007_03_29_
#define _GWK_BASE_IMPL_H__INCLUDED__2007_03_29_

#include <list>
#include <set>
#include <utility>
#include <vector>

using std::list;
using std::set;
using std::multiset;
using std::pair;
using std::vector;

#include "GwkBase.h"
#include "util.h"

class CGwkBaseImpl
{
public:
	CGwkBaseImpl();
	virtual ~CGwkBaseImpl();

public:
	typedef CGwkBase::GwkSearchStrategy GwkSearchStrategy;
	HGWK_RESULT Search(const CGwkNode* pSrc,
					   const std::vector<const CGwkNode*> vDst,
					   const std::vector<const CGwkNode*> vObstacle,
					   GwkSearchStrategy strategy,
					   unsigned int strategy_param);

	static const CGwkNode* GetFirstNode(HGWK_RESULT);
	static const CGwkNode* GetLastNode(HGWK_RESULT);
	static const CGwkNode* GetNextNode(HGWK_RESULT);
	static const CGwkNode* GetPrevNode(HGWK_RESULT);
	static void ReleaseResult(HGWK_RESULT &);

public:
	class CNodeWrapper
	{
	public:
		CNodeWrapper(const CNodeWrapper* parent);
		virtual ~CNodeWrapper();

	public:
		const CNodeWrapper* GetParent() const;

	public:
		virtual bool UniqueLess(const CNodeWrapper &) const = 0;
		virtual bool ScoreLess(const CNodeWrapper &) const = 0;
		virtual const CGwkNode* GetNode() const = 0;
		virtual bool LevelDown(const vector<const CGwkNode*> &vDst,
							   vector<const CNodeWrapper*> &) const = 0;

		virtual bool UniqueEqual(const CNodeWrapper &) const;

	private:
		const CNodeWrapper* parent;
	};

public:
	virtual const CNodeWrapper* WrapNode(const CGwkNode*) const = 0;

public:
	template<typename Nodes>
	// Nodes should be a type as vector<const CGwkNode*> or
	// list<const CGwkNode*> or set<const CGwkNode*>
	static void ReleaseNodes(Nodes &nodes)
	{
		for(typename Nodes::iterator it=nodes.begin(); it!=nodes.end(); ++it)
		{
			CGwkNode *pNode = const_cast<CGwkNode*>(*it);
			ASSERT(pNode != NULL);
			delete pNode;
		}
		nodes.clear();
	}
	template<typename WNodes>
	// WNodes should be a type as vector<const CNodeWrapper*> or
	// list<const CNodeWrapper*> or set<const CNodeWrapper*>
	static void ReleaseWNodes(WNodes &wnodes)
	{
		for(typename WNodes::iterator it=wnodes.begin(); it!=wnodes.end(); ++it)
		{
			CNodeWrapper *pWNode = const_cast<CNodeWrapper *>(*it);
			ASSERT(pWNode != NULL);
			delete pWNode;
		}
		wnodes.clear();
	}

protected:
	static HGWK_RESULT NewResult();
	static bool AppendNodeToResult(HGWK_RESULT, const CNodeWrapper*);
	static bool PrependNodeToResult(HGWK_RESULT, const CNodeWrapper*);

private:
	class CGwkResult
	{
	public:
		CGwkResult();
		virtual ~CGwkResult();

	public:
		const CGwkNode* GetFirstNode();
		const CGwkNode* GetLastNode();
		const CGwkNode* GetNextNode();
		const CGwkNode* GetPrevNode();
		bool push_front(const CNodeWrapper*);
		bool push_back(const CNodeWrapper*);

	protected:
		typedef list<const CNodeWrapper*> WNodes;
		WNodes path;
		bool freezed;
		bool have_got_the_first;
		WNodes::const_iterator cur_visited_node;
	};

private:
	class NodeSet
	{
	public:
		NodeSet();
		~NodeSet();

	public:
		bool FromNodes(const CGwkBaseImpl* pGwkImpl,
					   const std::vector<const CGwkNode*> &);

	public:
		bool Empty() const;
		bool InSet(const CNodeWrapper*) const;

		// caller should make sure the node being insert did not include in the set
		bool InsertNode(const CNodeWrapper*);

		// caller should make sure the node being insert do included in the set
		const CNodeWrapper* PopNode(const CNodeWrapper*);

		// return true if the the ARG node has higher score than the node in the set
		// caller should make sure the node being insert do included in the set
		bool HasHigherScore(const CNodeWrapper*) const;
		const CNodeWrapper* PopHighestScoreNode();

		bool ClearAll();

	public:
		typedef const CNodeWrapper* Node;
		struct UniqueLess
		{
			bool operator()(const Node&, const Node&) const;
		};
		struct ScoreLess
		{
			bool operator()(const Node&, const Node&) const;
		};

		typedef set<Node, UniqueLess> UniqueSet;
		typedef multiset<Node, ScoreLess> ScoreSet;

	protected:
		UniqueSet m_us;
		ScoreSet m_ss;
	};

	class ObsoleteNodeSet
	{
	public:
		ObsoleteNodeSet();
		~ObsoleteNodeSet();

	public:
		bool InSet(const CNodeWrapper*) const;

		// caller should make sure the node being insert did not include in the set
		bool InsertNode(const CNodeWrapper*);

		// caller should make sure the node being insert do included in the set
		const CNodeWrapper* PopNode(const CNodeWrapper*);

		bool ClearAll();

	public:
		typedef const CNodeWrapper* Node;
		struct UniqueLess
		{
			bool operator()(const Node&, const Node&) const;
		};

		typedef set<Node, UniqueLess> UniqueSet;

	protected:
		UniqueSet m_us;
	};
};

#endif // _GWK_BASE_IMPL_H__INCLUDED__2007_03_29_
