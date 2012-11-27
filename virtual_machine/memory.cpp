#include "memory.h"
#include "bytecodeconstants.h"

using namespace std;

Memory::Memory(): memory(), ints(), doubles()
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
	if(doubles.find(value) == doubles.end())
	{
		uint32_t ptr = allocate(cls);
		DoubleObject * o = (DoubleObject *) getObject(ptr);
		o->setValue(value);
		return ptr;
	} else
	{
		return doubles[value];
	}
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
	StringObject * str = new StringObject(cls);
	str->setValue(value);
	memory.push_back(str);
	return memory.size() - 1;
}

uint32_t Memory::allocateArray(Class * cls, uint32_t length)
{
	ArrayObject * o = new ArrayObject(cls, length);
	memory.push_back(o);
	return memory.size() - 1;
}
