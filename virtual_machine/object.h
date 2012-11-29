#ifndef OBJECT_H
#define OBJECT_H

#include "class.h"

class Object
{
public:
	Object(Class * type);
	Object(Class * type, uint32_t length);
	virtual ~Object();
	Class * getType() const;
	uint32_t getValue(uint32_t field) const;
	void setValue(uint32_t field, uint32_t value);

protected:
	Class * type;
	uint32_t * fields;
	uint32_t fieldCount;
	
	void allocateFields();
};

#endif // OBJECT_H
