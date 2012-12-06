#ifndef OBJECT_H
#define OBJECT_H

#include "class.h"

class Object
{
public:
	Object();
	Object(Class * type);
	Object(Class * type, uint32_t length);
	Object(const Object & other);
	Object & operator=(const Object & other);
	Class * getType() const;
	uint32_t getValue(uint32_t field) const;
	void setValue(uint32_t field, uint32_t value);
	uint32_t * getFields() const;
	void setFields(uint32_t * fields);
	uint32_t getFieldCount() const;
	
	uint32_t address;

protected:
	Class * type;
	uint32_t * fields;
	uint32_t fieldCount;
};

#endif // OBJECT_H
