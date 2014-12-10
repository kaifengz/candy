// GwkBase.cpp: implementation of the CGwkBase class.
//

#include "GwkBase.h"
#include "GwkBaseImpl.h"
#include "util.h"

CGwkBase::CGwkBase(CGwkBaseImpl* pImpl)
{
	ASSERT(pImpl != NULL);
	m_pImpl = pImpl;
}

CGwkBase::~CGwkBase()
{
	ASSERT(m_pImpl != NULL);
	delete m_pImpl;
	m_pImpl = NULL;
}

HGWK_RESULT CGwkBase::Search(const CGwkNode* pSrc,
							 const CGwkNode* pDst,
							 CGwkBase::GwkSearchStrategy strategy,
							 unsigned int strategy_param)
{
	std::vector<const CGwkNode*> vObstacle;
	return Search(pSrc, pDst, vObstacle, strategy, strategy_param);
}

HGWK_RESULT CGwkBase::Search(const CGwkNode* pSrc,
							 const CGwkNode* pDst,
							 const std::vector<const CGwkNode*> vObstacle,
							 GwkSearchStrategy strategy,
							 unsigned int strategy_param)
{
	std::vector<const CGwkNode*> vDst;
	if(pDst != NULL)
		vDst.push_back(pDst);
	return Search(pSrc, vDst, vObstacle, strategy, strategy_param);
}

HGWK_RESULT CGwkBase::Search(const CGwkNode* pSrc,
							 const std::vector<const CGwkNode*> vDst,
							 const std::vector<const CGwkNode*> vObstacle,
							 GwkSearchStrategy strategy,
							 unsigned int strategy_param)
{
	ASSERT(m_pImpl != NULL);
	return m_pImpl->Search(pSrc, vDst, vObstacle, strategy, strategy_param);
}

const CGwkNode* CGwkBase::GetFirstNode(HGWK_RESULT hResult)
{
	return CGwkBaseImpl::GetFirstNode(hResult);
}

const CGwkNode* CGwkBase::GetLastNode(HGWK_RESULT hResult)
{
	return CGwkBaseImpl::GetLastNode(hResult);
}

const CGwkNode* CGwkBase::GetNextNode(HGWK_RESULT hResult)
{
	return CGwkBaseImpl::GetNextNode(hResult);
}

void CGwkBase::ReleaseResult(HGWK_RESULT &hResult)
{
	CGwkBaseImpl::ReleaseResult(hResult);
}
