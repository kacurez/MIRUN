#include "class.h"

Class::Class(const char * name): name(name)
{
	//ctor
}

Class::~Class()
{
	//dtor
}

void Class::setFieldCount(uint8_t fieldCount)
{
	this->fieldCount = fieldCount;
}

int Class::getFieldCount() const
{
	return fieldCount;
}

const Function * Class::getMethod(uint8_t i) const
{
	return methods[i];
}

void Class::addMethod(Function * f)
{
	methods.push_back(f);
}

std::string Class::getName() const
{
	return name;
}

