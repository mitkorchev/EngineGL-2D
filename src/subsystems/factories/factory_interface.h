#pragma once
#include "../resource_service.h"

class FactoryInterface {

	const ResourceService* m_ResService = nullptr;

public:

	FactoryInterface() {}

	FactoryInterface(
		const ResourceService* res
	)
		: m_ResService(res)
	{}

public:

	const ResourceService* GetResService() const { return m_ResService; }
	virtual ~FactoryInterface() {}

};