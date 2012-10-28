#ifndef OBJECT_H
#define OBJECT_H

#include "class.h"

class Object
{
public:
	Object(Class * type);
	virtual ~Object();
	const Class * getType() const;
	uint32_t getValue(uint8_t field) const;
	void setValue(uint8_t field, uint32_t value);

private:
	Class * type;
	uint32_t * fields;
};

#endif // OBJECT_H
