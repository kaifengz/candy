
#ifndef _GWK_BEST_FIRST_H__INCLUDED__2007_04_08_
#define _GWK_BEST_FIRST_H__INCLUDED__2007_04_08_

#include <vector>
#include <utility>

#include "GwkBase.h"

class CGwkBestFirst: public CGwkBase
{
public:
	CGwkBestFirst();
};

class CGwkNode_BestFirst: public CGwkNode
{
public:
	typedef int cost_t;

	virtual int Compare(const CGwkNode &) const = 0;
	virtual bool LevelDown(std::vector<const CGwkNode*> &) const;
	virtual const CGwkNode* Clone() const = 0;

	virtual bool LevelDown(std::vector<std::pair<const CGwkNode_BestFirst*, cost_t> > &) const = 0;
	virtual bool EstimateCost(const std::vector<const CGwkNode_BestFirst*> &vDst, cost_t &) const = 0;
};

#endif // _GWK_BEST_FIRST_H__INCLUDED__2007_04_08_
