#include "class.h"

Class::Class(const char * name): name(name)
{
	//ctor
}

Class::~Class()
{
	for(std::vector<Method *>::iterator it = methods.begin(); it != methods.end(); it ++)
	{
		delete *it;
	}
	methods.clear();
	delete pool;
}

void Class::setFieldCount(uint8_t fieldCount)
{
	this->fieldCount = fieldCount;
}

int Class::getFieldCount() const
{
	return fieldCount;
}

const Method * Class::getMethod(uint8_t i) const
{
	return methods[i];
}

void Class::addMethod(Method * f)
{
	methods.push_back(f);
}

std::string Class::getName() const
{
	return name;
}

void Class::setConstPool(ConstantPool * pool)
{
	this->pool = pool;
}

