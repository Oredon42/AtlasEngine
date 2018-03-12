#ifndef A_SYSTEM_H
#define A_SYSTEM_H

#include <vector>
#include <unordered_map>

class AEntity;

class ASystem
{
public:
	ASystem();
	virtual ~ASystem();

	bool isEntityCompatible(const AEntity *pEntity);

	bool hasEntity(const AEntity *pEntity);
	void updateEntity(AEntity *pEntity);

private:
	std::vector<size_t> m_vNeededComponentsTypesHashCodes;
	std::unordered_map<unsigned int, AEntity *> m_mappEntities;
};

#endif // A_SYSTEM_H
