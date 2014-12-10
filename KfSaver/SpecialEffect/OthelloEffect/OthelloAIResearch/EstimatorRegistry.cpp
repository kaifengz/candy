#include "StdAfx.h"
#include "EstimatorRegistry.h"

EstimatorRegistry::EstimatorRegistry()
{
}

EstimatorRegistry* EstimatorRegistry::GetInstance()
{
	static EstimatorRegistry registry;
	return &registry;
}

void EstimatorRegistry::RegisterEstimator(const char* name, EstimatorCreator creator)
{
	m_creators[name] = creator;
}

IEstimator* EstimatorRegistry::CreateEstimator(const char* name)
{
	EstimatorCreators::const_iterator iter = m_creators.find(name);
	if (iter == m_creators.end())
		return NULL;

	if (iter->second == NULL)
		return NULL;

	return (iter->second)();
}

void EstimatorRegistry::GetAllEstimators(vector<string> &names)
{
	for (EstimatorCreators::const_iterator iter = m_creators.begin();
		iter != m_creators.end(); ++iter)
	{
		names.push_back(iter->first);
	}
}

EstimatorRegister::EstimatorRegister(
		const char* name, EstimatorRegistry::EstimatorCreator creator)
{
	EstimatorRegistry::GetInstance()->RegisterEstimator(name, creator);
}
