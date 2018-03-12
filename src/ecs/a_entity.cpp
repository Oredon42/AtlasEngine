#include "a_entity.h"

#include "a_component.h"

unsigned int AEntity::s_uiGlobalIndex = 0;

AEntity::AEntity() :
	m_uiIndex(s_uiGlobalIndex++)
{
	
}

AEntity::~AEntity()
{

}

bool AEntity::addComponent(AComponent *pComponent)
{
	const size_t &typeHashCode = pComponent->getTypeHashCode();
	
	if(m_mappComponents.find(typeHashCode) == m_mappComponents.end())
	{
		m_mappComponents[typeHashCode] = pComponent;

		return true;
	}

	return false;
}

AComponent *AEntity::getComponent(const size_t &typeHashCode)
{
	if(m_mappComponents.find(typeHashCode) != m_mappComponents.end())
	{
		return m_mappComponents.at(typeHashCode);
	}
	else
	{
		return nullptr;
	}
}

AComponent *AEntity::getComponentConst(const size_t &typeHashCode) const
{
	if (m_mappComponents.find(typeHashCode) != m_mappComponents.end())
	{
		return m_mappComponents.at(typeHashCode);
	}
	else
	{
		return nullptr;
	}
}
