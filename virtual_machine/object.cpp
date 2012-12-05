#include "object.h"
#include <string.h>

Object::Object(Class * type): type(type), fields(0)
{
	fieldCount = type->getFieldCount();
	allocateFields();
}

Object::~Object()
{
	delete [] fields;
}

void Object::setValue(uint32_t field, uint32_t value)
{
	if(field >= fieldCount)
	{
		throw "No such field.";
	}
	fields[field] = value;
}

uint32_t Object::getValue(uint32_t field) const
{
	if(field >= fieldCount)
	{
		throw "No such field.";
	}
	return fields[field];
}

Class * Object::getType() const
{
	return type;
}

Object::Object(Class * type, uint32_t length): type(type), fields(0), fieldCount(length)
{
	allocateFields();
}

void Object::allocateFields()
{
	if(fieldCount)
	{
		fields = new uint32_t[fieldCount];
		memset(fields, 0, fieldCount * sizeof(uint32_t));
	}
}


