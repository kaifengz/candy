// GwkBestFirst.cpp: implementation of the CGwkBestFirst class.
//

#include "GwkBestFirst.h"
#include "GwkBestFirstImpl.h"
#include "util.h"

CGwkBestFirst::CGwkBestFirst() :
	CGwkBase(new CGwkBestFirstImpl())
{
}

////////////////////////////////////////////////////////////////////////////////
//	CGwkBestFirstNode
////////////////////////////////////////////////////////////////////////////////

bool CGwkNode_BestFirst::LevelDown(vector<const CGwkNode*> &nodes) const
{
	typedef vector<pair<const CGwkNode_BestFirst*, cost_t> > pairs_t;

	pairs_t pairs;
	if(!LevelDown(pairs))
		return false;

	ASSERT(nodes.empty());
	for(pairs_t::iterator iter=pairs.begin(); iter!=pairs.end(); ++iter)
		nodes.push_back(static_cast<const CGwkNode*>(iter->first));
	pairs.clear();

	return true;
}
