#include "a_system.h"

#include "a_entity.h"

ASystem::ASystem()
{

}

ASystem::~ASystem()
{

}

bool ASystem::isEntityCompatible(const AEntity *pEntity)
{
	for(size_t i = 0; i < m_vNeededComponentsTypesHashCodes.size(); ++i)
	{
		if(pEntity->getComponentConst(m_vNeededComponentsTypesHashCodes[i]) == nullptr)
		{
			return false;
		}
	}

	return true;
}

bool ASystem::hasEntity(const AEntity *pEntity)
{
	return m_mappEntities.find(pEntity->getIndex()) != m_mappEntities.end();
}

void ASystem::updateEntity(AEntity *pEntity)
{
	const bool &bIsEntityCompatible = isEntityCompatible(pEntity);

	if(hasEntity(pEntity) == true)
	{
		if(bIsEntityCompatible == false)
		{
			m_mappEntities.erase(pEntity->getIndex());
		}
	}
	else if(bIsEntityCompatible == true)
	{
		m_mappEntities[pEntity->getIndex()] = pEntity;
	}
}