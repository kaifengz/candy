
#include "StdAfx.h"
#include "EffectRegister.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

class EffectRegistryImpl
{
protected:
	EffectRegistryImpl();
public:
	~EffectRegistryImpl();

public:
	typedef EffectRegistry::CreateEffectFunc CreateEffectFunc;
	typedef EffectRegistry::EffectCreators EffectCreators;

	static void RegisterEffect(
		CreateEffectFunc func,
		const char* name);
	static const EffectCreators& GetEffectCreators();

#ifdef DEBUG_MODE
	static void RegisterEffectForTest(
		CreateEffectFunc func,
		const char* name,
		const char* date);
	static CreateEffectFunc GetCreatorForTest();
#endif

protected:
	static EffectRegistryImpl& GetInstance();

protected:
	EffectCreators m_ceators;

#ifdef DEBUG_MODE
	CreateEffectFunc m_tester;
	const char* m_tester_date;
#endif
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

EffectRegistryImpl::EffectRegistryImpl()
{
#ifdef DEBUG_MODE
	m_tester = NULL;
	m_tester_date = NULL;
#endif
}

EffectRegistryImpl::~EffectRegistryImpl()
{
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void EffectRegistryImpl::RegisterEffect(
		CreateEffectFunc func,
		const char* name)
{
	ASSERT(func != NULL);
	ASSERT(name != NULL);

	EffectRegistryImpl &instance = GetInstance();
	instance.m_ceators.push_back(func);

	LOG("SpecialEffect #%02d: %s", instance.m_ceators.size(), name);
}

const EffectRegistryImpl::EffectCreators& EffectRegistryImpl::GetEffectCreators()
{
	return GetInstance().m_ceators;
}

#ifdef DEBUG_MODE
void EffectRegistryImpl::RegisterEffectForTest(
	CreateEffectFunc func,
	const char* name,
	const char* date)
{
	ASSERT(func != NULL);
	ASSERT(name != NULL);
	ASSERT(date != NULL);

	EffectRegistryImpl &instance = GetInstance();
	LOG("SpecialEffect [TEST]: %s -- %s", name, date);

	if (instance.m_tester == NULL)
	{
		instance.m_tester = func;
		instance.m_tester_date = date;
	}
	else
	{
		ASSERT(instance.m_tester_date != NULL);
		if (strcmp(date, instance.m_tester_date) > 0)
		{
			instance.m_tester = func;
			instance.m_tester_date = date;
		}
	}
}

EffectRegistryImpl::CreateEffectFunc EffectRegistryImpl::GetCreatorForTest()
{
	return GetInstance().m_tester;
}
#endif

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

EffectRegistryImpl& EffectRegistryImpl::GetInstance()
{
	static EffectRegistryImpl* pImpl = NULL;

	if (pImpl == NULL)
		pImpl = new EffectRegistryImpl();
	ASSERT(pImpl != NULL);
	return *pImpl;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void EffectRegistry::RegisterEffect(
		CreateEffectFunc func,
		const char* name)
{
	EffectRegistryImpl::RegisterEffect(func, name);
}

const EffectRegistry::EffectCreators& EffectRegistry::GetEffectCreators()
{
	return EffectRegistryImpl::GetEffectCreators();
}

#ifdef DEBUG_MODE
void EffectRegistry::RegisterEffectForTest(
		CreateEffectFunc func,
		const char* name,
		const char* date)
{
	EffectRegistryImpl::RegisterEffectForTest(func, name, date);
}

EffectRegistry::CreateEffectFunc EffectRegistry::GetCreatorForTest()
{
	return EffectRegistryImpl::GetCreatorForTest();
}
#endif
