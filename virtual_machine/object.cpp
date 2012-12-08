#include "object.h"
#include <string.h>

Object::Object(Class * type): address(VM_NULL), type(type), fields(0)
{
	fieldCount = type->getFieldCount();
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

Object::Object(Class * type, uint32_t length): address(VM_NULL), type(type), fields(0), fieldCount(length)
{
	
}

 Object::Object(const Object & other)
{
	type = other.type;
	fieldCount = other.fieldCount;
	fields = other.fields;
	address = address;
}

Object::Object(): type(0), fields(0), fieldCount(0)
{
	
}

void Object::setFields(uint32_t * fields)
{
	this->fields = fields;
}

uint32_t * Object::getFields() const
{
	return fields;
}

Object & Object::operator= (const Object & other)
{
	if(this != &other)
	{
		type = other.type;
		fields = other.fields;
		fieldCount = other.fieldCount;
		address = address;
	}
	return * this;
}

uint32_t Object::getFieldCount() const
{
	return fieldCount;
}

void Object::setValueByName(const char * field, uint32_t value)
{
	fields[type->getFieldIndex(field)] = value;
}

uint32_t Object::getValueByName(const char * field) const
{
	return fields[type->getFieldIndex(field)];
}

