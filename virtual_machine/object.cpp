#include "object.h"
#include <string.h>

Object::Object(Class * type): type(type), fields(0)
{
	if(type->getFieldCount())
	{
		fields = new uint32_t[type->getFieldCount()];
		memset(fields, 0, type->getFieldCount() * sizeof(uint32_t));
	}
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

Class * Object::getType() const
{
	return type;
}

double DoubleObject::getValue() const
{
	return value;
}

void DoubleObject::setValue(double value)
{
	this->value = value;
}

DoubleObject::~DoubleObject()
{

}

DoubleObject::DoubleObject(Class * type): Object(type), value(0)
{

}

const char * StringObject::getValue() const
{
	return value;
}

 StringObject::~StringObject()
{
	delete value;
}

StringObject::StringObject(Class * type): Object(type), value(0) 
{
}

void StringObject::setValue(const char * value)
{
	if(value)
	{
		int length = strlen(value) + 1;
		this->value = new char[length];
		strcpy(this->value, value);
	}
}

void ArrayObject::setValue(uint32_t field, uint32_t value)
{
	if(field >= length)
	{
		throw 5;
	}
	fields[field] = value;
}

uint32_t ArrayObject::getValue(uint32_t field) const
{
	if(field >= length)
	{
		throw 5;
	}
	return fields[field];
}

ArrayObject::ArrayObject(Class * type, uint32_t length): Object(type), length(length)
{
	fields = new uint32_t[length];
	memset(fields, 0, length * sizeof(uint32_t));
}


