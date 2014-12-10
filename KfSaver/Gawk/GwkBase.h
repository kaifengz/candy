
#ifndef _BASE_H__INCLUDED__2007_04_05_
#define _BASE_H__INCLUDED__2007_04_05_

#include <vector>

typedef void* HGWK_RESULT;
class CGwkBase;
class CGwkBaseImpl;
class CGwkNode;

class CGwkBase
{
protected:
	CGwkBase(CGwkBaseImpl*);
public:
	virtual ~CGwkBase();

public:
	enum GwkSearchStrategy
	{
		GSS_REACH_DST = 0,
		GSS_VISITED_NODE_COUNT,
		GSS_ELAPSED_TIME,

		GSS_MAX
	};
	HGWK_RESULT Search(const CGwkNode* pSrc,
					   const CGwkNode* pDst,
					   GwkSearchStrategy strategy=GSS_REACH_DST,
					   unsigned int strategy_param=0);
	HGWK_RESULT Search(const CGwkNode* pSrc,
					   const CGwkNode* pDst,
					   const std::vector<const CGwkNode*> vObstacle,
					   GwkSearchStrategy strategy=GSS_REACH_DST,
					   unsigned int strategy_param=0);
	HGWK_RESULT Search(const CGwkNode* pSrc,
					   const std::vector<const CGwkNode*> vDst,
					   const std::vector<const CGwkNode*> vObstacle,
					   GwkSearchStrategy strategy=GSS_REACH_DST,
					   unsigned int strategy_param=0);

public:
	static const CGwkNode* GetFirstNode(HGWK_RESULT);
	static const CGwkNode* GetLastNode(HGWK_RESULT);
	static const CGwkNode* GetNextNode(HGWK_RESULT);
	static const CGwkNode* GetPrevNode(HGWK_RESULT);
	static void ReleaseResult(HGWK_RESULT &);

private:
	CGwkBaseImpl *m_pImpl;
};

class CGwkNode
{
public:
	virtual int Compare(const CGwkNode &) const = 0;
	virtual bool LevelDown(std::vector<const CGwkNode*> &) const = 0;
	virtual const CGwkNode* Clone() const = 0;
};

#endif // _BASE_H__INCLUDED__2007_04_05_
