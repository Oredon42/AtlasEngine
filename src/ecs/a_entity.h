#ifndef A_ENTITY_H
#define A_ENTITY_H

#include <unordered_map>

class AComponent;

class AEntity
{
private:
	static unsigned int s_uiGlobalIndex;

public:
	AEntity();
	virtual ~AEntity();

	inline unsigned int getIndex() const{return m_uiIndex;}

	bool addComponent(AComponent *pComponent);

	AComponent *getComponent(const size_t &typeHashCode);
	AComponent *getComponentConst(const size_t &typeHashCode) const;

protected:
	unsigned int m_uiIndex;

	std::unordered_map<size_t, AComponent *> m_mappComponents;
};

#endif // A_ENTITY_H
