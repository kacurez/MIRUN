#include "object.h"
#include <string.h>

Object::Object(Class * type): type(type)
{
	fields = new uint32_t[type->getFieldCount()];
	memset(fields, 0, type->getFieldCount() * sizeof(uint32_t));
}

Object::~Object()
{
	delete [] fields;
}

void Object::setValue(uint8_t field, uint32_t value)
{
	if(field >= type->getFieldCount())
	{
		throw 5;
	}
	fields[field] = value;
}

uint32_t Object::getValue(uint8_t field) const
{
	if(field >= type->getFieldCount())
	{
		throw 5;
	}
	return fields[field];
}

const Class * Object::getType() const
{
	return type;
}