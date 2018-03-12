#include "a_component.h"

#include <typeinfo>

AComponent::AComponent()
{

}

AComponent::~AComponent()
{

}

size_t AComponent::getTypeHashCode() const
{
	return typeid(*this).hash_code();
}