#ifndef A_COMPONENT_H
#define A_COMPONENT_H

class AComponent
{
public:
	AComponent();
	virtual ~AComponent();

	inline virtual size_t getTypeHashCode() const;
};

#endif // A_COMPONENT_H
