#include "memory.h"
#include "bytecodeconstants.h"
#include <string.h>

using namespace std;

Memory::Memory(): memory(), ints(), doubles(), strings()
{
	//ctor
}

Memory::~Memory()
{
	for(vector<Object *>::iterator it = memory.begin(); it < memory.end(); it ++)
	{
		delete (* it);
	}
	memory.clear();
	ints.clear();
	doubles.clear();
	for(vector<char *>::iterator it = strings.begin(); it < strings.end(); it ++)
	{
		delete [] (*it);
	}
	strings.clear();
}

uint32_t Memory::allocate(Class * cls)
{
	Object * o = new Object(cls);
	memory.push_back(o);
	return memory.size() - 1;
}

Object * Memory::getObject(uint32_t pointer) const
{
	if(pointer >= memory.size())
	{
		throw "Memory violation.";
	}
	return memory[pointer];
}

uint32_t Memory::allocateNumber(Class * cls, double value)
{
	//TODO same number should have same reference
	uint32_t ptr = allocate(cls);
	Object * o = getObject(ptr);
	doubles.push_back(value);
	o->setValue(0, doubles.size() - 1);
	return ptr;
}

uint32_t Memory::allocateNumber(Class * cls, int32_t value)
{
	if(cls->getName() == REAL_CLASS)
	{
		return allocateNumber(cls, (double) value);
	}
	if(ints.find(value) == ints.end())
	{
		uint32_t ptr = allocate(cls);
		Object * o = getObject(ptr);
		o->setValue(0, value);
		return ptr;
	} else
	{
		return ints[value];
	}
}

uint32_t Memory::allocateString(Class * cls, const char * value)
{
	//TODO intern strings
	uint32_t ptr = allocate(cls);
	Object * str = getObject(ptr);
	int length = strlen(value);
	char * copy = new char[length];
	strcpy(copy, value);
	strings.push_back(copy);
	str->setValue(0, strings.size() - 1);
	return ptr;
}

uint32_t Memory::allocateArray(Class * cls, uint32_t length)
{
	Object * o = new Object(cls, length);
	memory.push_back(o);
	return memory.size() - 1;
}

const char * Memory::getStringValue(uint32_t ref) const
{
	if(ref >= strings.size())
	{
		throw "Memory violation.";
	}
	return strings[ref];
}

double Memory::getDoubleValue(uint32_t ref) const
{
	if(ref >= doubles.size())
	{
		throw "Memory violation.";
	}
	return doubles[ref];
}


