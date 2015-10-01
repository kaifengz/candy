// EffectFactory.cpp: implementation of the CEffectFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectFactory.h"
#include "EffectRegister.h"

#ifdef DEBUG_MODE
// #define DEFAULT_STRATEGY S_LATEST_FOR_TEST
#define DEFAULT_STRATEGY S_SEQUENTIAL_ALL
#else
#define DEFAULT_STRATEGY S_RANDOM_CONFIGURED
#endif

CEffectCreationStrategy* CEffectFactory::m_strategy = NULL;

CSpecialEffect* CEffectFactory::CreateEffect(
		HWND hWnd,
		HBITMAP hBmpBkgnd)
{
	CSpecialEffect *pEffect = m_strategy->CreateEffect(hWnd, hBmpBkgnd);
	if(pEffect != NULL)
		LOG("Create %s", pEffect->GetEffectName());
	return pEffect;
}

void CEffectFactory::DeleteEffect(CSpecialEffect *&pEffect)
{
	if(pEffect != NULL)
	{
		LOG("Delete %s", pEffect->GetEffectName());

		delete pEffect;
		pEffect = NULL;
	}
}

void CEffectFactory::Initialize()
{
	srand(GetTickCount());
	SetCreationStrategy(DEFAULT_STRATEGY);
}

class Strategy_LatestForTest : public CEffectCreationStrategy
{
public:
	virtual const char* GetStrategyName() const
	{
		return "LastestForTest";
	}
	virtual CSpecialEffect* CreateEffect(HWND hWnd, HBITMAP hBmpBkgnd)
	{
		EffectRegistry::CreateEffectFunc creator = EffectRegistry::GetCreatorForTest();
		if (creator == NULL)
			return NULL;

		return creator(hWnd, hBmpBkgnd);
	}
};

class Strategy_Random : public CEffectCreationStrategy
{
public:
	virtual CSpecialEffect* CreateEffect(HWND hWnd, HBITMAP hBmpBkgnd)
	{
		if (m_creators.empty())
		{
			m_creators = GetCreators();
			if (m_creators.empty())
				return NULL;

			m_weights.clear();
			m_weights.assign(m_creators.size(), 0);
		}

		unsigned int iEffect = RandomSelect(m_weights);
		ASSERT(iEffect >= 0 && iEffect < m_weights.size());

		EffectRegistry::CreateEffectFunc creator = m_creators[iEffect];
		ASSERT(creator != NULL);
		return creator(hWnd, hBmpBkgnd);
	}

	virtual const EffectRegistry::EffectCreators& GetCreators() = 0;

private:
	EffectRegistry::EffectCreators m_creators;
	std::vector<unsigned int> m_weights;
};

class Strategy_Sequential : public CEffectCreationStrategy
{
public:
	virtual CSpecialEffect* CreateEffect(HWND hWnd, HBITMAP hBmpBkgnd)
	{
		if (m_creators.empty())
		{
			m_creators = GetCreators();
			if (m_creators.empty())
				return NULL;

			m_index = random(m_creators.size());
		}

		EffectRegistry::CreateEffectFunc creator = m_creators[m_index];
		m_index = (m_index + 1) % m_creators.size();

		ASSERT(creator != NULL);
		return creator(hWnd, hBmpBkgnd);
	}

	virtual const EffectRegistry::EffectCreators& GetCreators() = 0;

private:
	EffectRegistry::EffectCreators m_creators;
	int m_index;
};

template <class BaseStrategy>
class Strategy_Named : public BaseStrategy
{
protected:
	const char* const m_name;

public:
	explicit Strategy_Named(const char* name) :
		m_name(name)
	{
	}

	virtual const char* GetStrategyName() const
	{
		return m_name;
	}
};

template <class BaseStrategy>
class Strategy_Configured : public BaseStrategy
{
	virtual const EffectRegistry::EffectCreators& GetCreators()
	{
		return EffectRegistry::GetConfiguredEffectCreators();
	}
};

template <class BaseStrategy>
class Strategy_All : public BaseStrategy
{
public:
	virtual const EffectRegistry::EffectCreators& GetCreators()
	{
		return EffectRegistry::GetEffectCreators();
	}
};

BOOL CEffectFactory::SetCreationStrategy(ZEffectCreationStrategy strategy)
{
	if (m_strategy != NULL)
	{
		delete m_strategy;
		m_strategy = NULL;
	}

	switch (strategy)
	{
	case S_RANDOM_CONFIGURED:
		m_strategy = new Strategy_Named<Strategy_Configured<Strategy_Random>>("RandomConfigured");
		break;
	case S_RANDOM_ALL:
		m_strategy = new Strategy_Named<Strategy_All<Strategy_Random>>("RandomAll");
		break;
	case S_SEQUENTIAL_CONFIGURED:
		m_strategy = new Strategy_Named<Strategy_Configured<Strategy_Sequential>>("SequentialConfigured");
		break;
	case S_SEQUENTIAL_ALL:
		m_strategy = new Strategy_Named<Strategy_All<Strategy_Sequential>>("SequentialAll");
		break;
	case S_LATEST_FOR_TEST:
		m_strategy = new Strategy_LatestForTest();
		break;
	}

	if (m_strategy == NULL)
		return FALSE;

	LOG("Apply strategy %s", m_strategy->GetStrategyName());
	return TRUE;
}
