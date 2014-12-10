
#include "GwkBase.h"
#include "GwkBreadthFirst.h"
#include "GwkBestFirst.h"
#include "util.h"

class CPlaneNode: public CGwkNode_BestFirst
{
protected:
	int x, y;

public:
	CPlaneNode(int _x, int _y) :
		x(_x), y(_y)
	{
	}
	virtual int Compare(const CGwkNode &another) const
	{
		const CPlaneNode *pAnother = dynamic_cast<const CPlaneNode*>(&another);
		ASSERT(pAnother != NULL);
		return	(x != pAnother->x) ? (x - pAnother->x) :
				(y != pAnother->y) ? (y - pAnother->y) :
				0;
	}
	virtual bool LevelDown(std::vector<const CGwkNode*> &nodes) const
	{
		ASSERT(nodes.empty());
		static const int offset_x[] = {1,0,-1,0};
		static const int offset_y[] = {0,1,0,-1};
		for(int i=0; i<4; i++)
			nodes.push_back(new CPlaneNode(x+offset_x[i], y+offset_y[i]));
		return true;
	}
	virtual const CGwkNode* Clone() const
	{
		return new CPlaneNode(x, y);
	}
	virtual bool LevelDown(std::vector<std::pair<const CGwkNode_BestFirst*, cost_t> > &pairs) const
	{
		ASSERT(pairs.empty());
		static const int offset_x[] = {1,0,-1,0};
		static const int offset_y[] = {0,1,0,-1};
		typedef std::pair<const CGwkNode_BestFirst*, cost_t> pair_t;
		for(int i=0; i<4; i++)
			pairs.push_back(pair_t(new CPlaneNode(x+offset_x[i], y+offset_y[i]), 1));
		return true;
	}
	virtual bool EstimateCost(
		const std::vector<const CGwkNode_BestFirst*> &vDst,
		cost_t &cost) const
	{
		ASSERT(vDst.size() > 0);
		const CPlaneNode *pDst = dynamic_cast<const CPlaneNode *>(vDst.front());
		cost = abs(x-pDst->x) + abs(y-pDst->y);
		return true;
	}
};

class CPlanePPNode: public CPlaneNode
{
public:
	CPlanePPNode(int _x, int _y): CPlaneNode(_x, _y)
	{
	}

	virtual const CGwkNode* Clone() const
	{
		return new CPlanePPNode(x, y);
	}
	virtual bool LevelDown(std::vector<const CGwkNode*> &nodes) const
	{
		ASSERT(nodes.empty());
		static const int offset_x[] = {1,0,-1,0};
		static const int offset_y[] = {0,1,0,-1};
		for(int i=0; i<4; i++)
		{
			if(x+offset_x[i]>=0 && y+offset_y[i]>=0)
				nodes.push_back(new CPlanePPNode(x+offset_x[i], y+offset_y[i]));
		}
		return true;
	};
};

template<typename CGawk>
class CGawkTester
{
public:
	template<typename CNode>
	void operator()(const CNode* pSrc, const CNode* pDst) const
	{
		CGawk gawk;
		HGWK_RESULT hResult = gawk.Search(pSrc, pDst);
		ASSERT(hResult != NULL);
		for(const CNode*p = dynamic_cast<const CNode*>(gawk.GetFirstNode(hResult));
			p != NULL;
			p = dynamic_cast<const CNode*>(gawk.GetNextNode(hResult)))
		{
			int i = 0;
		}
		gawk.ReleaseResult(hResult);
	}
};

void TestGawk()
{
	const int total = 1;

	LOG("Start test CGwkBreadthFirst...");
	for(int i=0; i<total; i++)
	{
		CGawkTester<CGwkBreadthFirst>()(new CPlaneNode(0,0), new CPlaneNode(10,10));
		CGawkTester<CGwkBreadthFirst>()(new CPlanePPNode(0,0), new CPlanePPNode(10,10));
		int j=0;
	}
	LOG("End test CGwkBreadthFirst");
	LOG("Start test CGwkBestFirst...");
	for(int i=0; i<total; i++)
	{
		CGawkTester<CGwkBestFirst>()(new CPlaneNode(0,0), new CPlaneNode(10,10));
		CGawkTester<CGwkBestFirst>()(new CPlanePPNode(0,0), new CPlanePPNode(10,10));
		int j=0;
	}
	LOG("End test CGwkBestFirst");
}
