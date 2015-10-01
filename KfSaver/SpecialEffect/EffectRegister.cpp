
#include "StdAfx.h"
#include "EffectRegister.h"

#include <memory>

static const char* const SYSTEM_REGISTRY_PATH = "SOFTWARE\\MySaver";

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

	void RegisterEffect(
		CreateEffectFunc func,
		const char* name);
	const EffectCreators& GetEffectCreators();
	const EffectCreators& GetConfiguredEffectCreators();

	void RegisterEffectForTest(
		CreateEffectFunc func,
		const char* name,
		const char* date);
	CreateEffectFunc GetCreatorForTest();

public:
	static EffectRegistryImpl& GetInstance();

protected:
	void LoadConfiguration();
	void LoadConfigurationFromSystemRegistry();
	void StoreConfigurationToSystemRegistry();

protected:
	typedef std::map<std::string, CreateEffectFunc> EffectCreatorMap;
	EffectCreatorMap m_creator_map;
	EffectCreators m_creators;
	EffectCreators m_configured_creators;

	CreateEffectFunc m_tester;
	const char* m_tester_date;
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

EffectRegistryImpl::EffectRegistryImpl()
{
	m_tester = NULL;
	m_tester_date = NULL;
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

	m_creator_map[name] = func;
	m_creators.push_back(func);
	LOG("SpecialEffect #%02d: %s", m_creator_map.size(), name);
}

const EffectRegistryImpl::EffectCreators& EffectRegistryImpl::GetEffectCreators()
{
	return m_creators;
}

const EffectRegistryImpl::EffectCreators& EffectRegistryImpl::GetConfiguredEffectCreators()
{
	LoadConfiguration();
	return m_configured_creators;
}

void EffectRegistryImpl::LoadConfiguration()
{
	m_configured_creators.clear();
	LoadConfigurationFromSystemRegistry();

	if (m_configured_creators.empty())
	{
		m_configured_creators = m_creators;
		StoreConfigurationToSystemRegistry();
	}
}

void EffectRegistryImpl::RegisterEffectForTest(
	CreateEffectFunc func,
	const char* name,
	const char* date)
{
	ASSERT(func != NULL);
	ASSERT(name != NULL);
	ASSERT(date != NULL);

	LOG("SpecialEffect [TEST]: %s -- %s", name, date);

	if (m_tester == NULL)
	{
		m_tester = func;
		m_tester_date = date;
	}
	else
	{
		ASSERT(m_tester_date != NULL);
		if (strcmp(date, m_tester_date) > 0)
		{
			m_tester = func;
			m_tester_date = date;
		}
	}
}

EffectRegistryImpl::CreateEffectFunc EffectRegistryImpl::GetCreatorForTest()
{
	return m_tester;
}

void EffectRegistryImpl::LoadConfigurationFromSystemRegistry()
{
	HKEY hKey = NULL;
	if (ERROR_SUCCESS != RegOpenKey(HKEY_CURRENT_USER, SYSTEM_REGISTRY_PATH, &hKey))
	{
		LOG("HKEY_CURRENT_USER\\%s could not be opened", SYSTEM_REGISTRY_PATH);
		return;
	}

	for (DWORD index = 0; ; ++index)
	{
		TCHAR name[128];
		DWORD name_len = sizeof(name);
		DWORD type;
		DWORD enabled;
		DWORD data_len = sizeof(enabled);

		LONG rc = RegEnumValue(hKey, index, name, &name_len, NULL, &type, (LPBYTE)&enabled, &data_len);
		if (rc == ERROR_NO_MORE_ITEMS)
			break;
		if (rc != ERROR_SUCCESS || type != REG_DWORD)
			continue;

		name[_countof(name) - 1] = '\0';
		EffectCreatorMap::const_iterator iter = m_creator_map.find(name);
		if (iter == m_creator_map.end())
		{
			LOG("SpecialEffect %s unknown", name);
			continue;
		}

		LOG("SpecialEffect %s is %s", name, enabled ? "enabled" : "disabled");
		if (enabled)
			m_configured_creators.push_back(iter->second);
	}

	RegCloseKey(hKey);
}

void EffectRegistryImpl::StoreConfigurationToSystemRegistry()
{
	LONG rc;

	HKEY hKey = NULL;
	if (ERROR_SUCCESS != (rc = RegOpenKey(HKEY_CURRENT_USER, SYSTEM_REGISTRY_PATH, &hKey)))
	{
		if (rc == ERROR_FILE_NOT_FOUND)
		{
			LOG("HKEY_CURRENT_USER\\%s does not exist, create it", SYSTEM_REGISTRY_PATH);
			if (ERROR_SUCCESS != (rc = RegCreateKey(HKEY_CURRENT_USER, SYSTEM_REGISTRY_PATH, &hKey)))
			{
				LOG("HKEY_CURRENT_USER\\%s could not be created: %d 0x%X", rc, rc);
				return;
			}
		}
		else
		{
			LOG("HKEY_CURRENT_USER\\%s could not be opened for adding configurations: %d 0x%X", rc, rc);
			return;
		}
	}

	for (auto iter : m_creator_map)
	{
		DWORD enabled = 1;
		if (ERROR_SUCCESS != (rc = RegSetValueEx(hKey, iter.first.c_str(), 0, REG_DWORD, (LPBYTE)&enabled, sizeof(enabled))))
		{
			LOG("HKEY_CURRENT_USER\\%s\\%s = 1 could not be written: %d 0x%X", SYSTEM_REGISTRY_PATH, iter.first.c_str(), rc, rc);
		}
	}

	RegCloseKey(hKey);
}

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
	EffectRegistryImpl::GetInstance().RegisterEffect(func, name);
}

const EffectRegistry::EffectCreators& EffectRegistry::GetEffectCreators()
{
	return EffectRegistryImpl::GetInstance().GetEffectCreators();
}

const EffectRegistry::EffectCreators& EffectRegistry::GetConfiguredEffectCreators()
{
	return EffectRegistryImpl::GetInstance().GetConfiguredEffectCreators();
}

void EffectRegistry::RegisterEffectForTest(
		CreateEffectFunc func,
		const char* name,
		const char* date)
{
	EffectRegistryImpl::GetInstance().RegisterEffectForTest(func, name, date);
}

EffectRegistry::CreateEffectFunc EffectRegistry::GetCreatorForTest()
{
	return EffectRegistryImpl::GetInstance().GetCreatorForTest();
}
