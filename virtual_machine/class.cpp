#include "class.h"

Class::Class(const char * name): name(name), pool(0)
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

int Class::getFieldCount() const
{
	return fields.size();
}

Method * Class::getMethod(uint8_t i) const
{
	return methods[i];
}

Method * Class::getMethod(const char * name) const
{
	for(std::vector<Method *>::const_iterator it = methods.begin(); it != methods.end(); it ++)
	{
		if((*it)->getName() == name)
		{
			return *it;
		}
	}
	throw "No such method.";
}

Method * Class::getMethod(const char * name, uint8_t paramCount) const
{
	for(std::vector<Method *>::const_iterator it = methods.begin(); it != methods.end(); it ++)
	{
		if((*it)->getName() == name && (*it)->getParamCount() == paramCount)
		{
			return *it;
		}
	}
	throw "No such method.";
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

ConstantPool* Class::getConstantPool() const
{
	return pool;
}

uint8_t Class::getFieldIndex(const char * name)
{
	if(fields.find(name) == fields.end())
	{
		throw "No such field.";
	}
	return fields[name];
}

void Class::addField(const char * name)
{
	fields[name] = fields.size();
}

bool Class::hasMethod(const char * name) const
{
	for(std::vector<Method *>::const_iterator it = methods.begin(); it != methods.end(); it ++)
	{
		if((*it)->getName() == name)
		{
			return true;
		}
	}
	return false;
}

std::string Class::getFieldName(uint8_t field) const
{
	for(std::map<std::string, uint8_t>::const_iterator it = fields.begin(); it != fields.end(); it ++)
	{
		if(it->second == field)
		{
			return it->first;
		}
	}
	throw "No such field.";
}

uint8_t Class::getMethodCount() const
{
	return methods.size();
}
