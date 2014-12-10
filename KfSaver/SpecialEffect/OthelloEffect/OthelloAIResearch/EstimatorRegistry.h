#pragma once

class IEstimator;

class EstimatorRegistry
{
public:
	static EstimatorRegistry* GetInstance();

public:
	typedef IEstimator* (*EstimatorCreator)();
	void RegisterEstimator(const char* name, EstimatorCreator);

	IEstimator* CreateEstimator(const char* name);

	void GetAllEstimators(vector<string> &names);

protected:
	EstimatorRegistry();
	EstimatorRegistry(const EstimatorRegistry &);

protected:
	typedef map<string, EstimatorCreator> EstimatorCreators;
	EstimatorCreators m_creators;
};

class EstimatorRegister
{
public:
	EstimatorRegister(const char* name, EstimatorRegistry::EstimatorCreator creator);
};
