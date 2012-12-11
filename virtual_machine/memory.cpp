#include "memory.h"
#include "bytecodeconstants.h"
#include <string.h>

#define MEMORY_SIZE 10000000

using namespace std;

Memory::Memory(): used(new char[MEMORY_SIZE]), empty(new char[MEMORY_SIZE]), size(0), maxSize(MEMORY_SIZE)
{
	objectSize = sizeof(Object);
}

Memory::~Memory()
{
	delete [] used;
	delete [] empty;
	ints.clear();
}

uint32_t Memory::allocate(Class * cls)
{
	Object o(cls);
	uint32_t fieldSpace = o.getFieldCount() * sizeof(uint32_t);
	if(!isSpace(fieldSpace + objectSize))
	{
		return VM_NULL;
	}
	if(fieldSpace > 0)
	{
		o.setFields((uint32_t *)(used + size));
		memset(o.getFields(), VM_NULL, fieldSpace);
		size += fieldSpace;
	}
	memcpy(used + size, &o, objectSize);
	uint32_t ret = size;
	size += objectSize;
	return ret;
}

Object * Memory::getObject(uint32_t pointer) const
{
	if(pointer >= size)
	{
		throw "Memory violation.";
	}
	return (Object *)(used + pointer);
}

uint32_t Memory::allocateNumber(Class * cls, double value)
{
	static unsigned int doubleSize = sizeof(double);
	uint32_t ptr = allocate(cls);
	if(ptr == VM_NULL || !isSpace(doubleSize))
	{
		return VM_NULL;
	}
	Object * o = getObject(ptr);
	memcpy(used + size, &value, sizeof(double));
	o->setValue(0, size);
	size += doubleSize;
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
		if(ptr ==VM_NULL)
		{
			return ptr;
		}
		Object * o = getObject(ptr);
		o->setValue(0, value);
		ints[value] = ptr;
		return ptr;
	} else
	{
		return ints[value];
	}
}

uint32_t Memory::allocateString(Class * cls, const char * value)
{
	unsigned int length = strlen(value) + 1;
	uint32_t ptr = allocate(cls);
	if(ptr ==VM_NULL || !isSpace(length))
	{
		return VM_NULL;
	}
	Object * str = getObject(ptr);
	memcpy(used + size, value, length);
	str->setValue(0, length);
	str->setValue(1, size);
	size += length;
	return ptr;
}

uint32_t Memory::allocateArray(Class * cls, uint32_t length)
{
	uint32_t space = length * sizeof(uint32_t);
	if(!isSpace(space + objectSize))
	{
		return VM_NULL;
	}
	Object o(cls, length);
	if(space > 0)
	{
		o.setFields((uint32_t *)(used + size));
		memset(o.getFields(), VM_NULL, space);
		size += space;
	}
	memcpy(used + size, &o, objectSize);
	uint32_t ret = size;
	size += objectSize;
	return ret;
}

const char * Memory::getStringValue(uint32_t ref) const
{
	if(ref >= size)
	{
		throw "Memory violation.";
	}
	return used + ref;
}

double Memory::getDoubleValue(uint32_t ref) const
{
	if(ref >= size)
	{
		throw "Memory violation.";
	}
	return *(double *)(used + ref);
}

FILE * Memory::getFileStream(uint32_t file)
{
	if(file >= size)
	{
		throw "No such file descriptor.";
	}
	FILE * ret;
	memcpy(&ret, used + file, sizeof(FILE *));
	return ret;
}

uint32_t Memory::createFile(const char * fileName, const char * mask)
{
	FILE * file = fopen(fileName, mask);
	if(file)
	{
		memcpy(used + size, &file, sizeof(FILE *));
		uint32_t ret = size;
		size += sizeof(FILE *);
		return ret;
	} else 
	{
		return VM_NULL;
	}
}

void Memory::gc(StackFrame * stack)
{
	StackFrame * sf = stack;
	size = 0;
	ints.clear();
	while(!moved.empty()) moved.pop();
	while(sf)
	{
		for(unsigned int i = 0; i < sf->size(); i ++)
		{
			sf->setValueAt(i, moveObject(sf->getValueAt(i)));
		}
		for(unsigned int i = 0; i < sf->getLocalCount(); i ++)
		{
			sf->setLocal(i, moveObject(sf->getLocal(i)));
		}
		sf = sf->getParent();
	}
	while(!moved.empty())
	{
		uint32_t ptr = moved.front(); 
		Object * o = (Object *)(empty + ptr);
		moved.pop();
		if(o->getType()->getName() == INT_CLASS)
		{
			ints[(int)o->getValue(0)] = ptr;
		} else if(o->getType()->getName() == REAL_CLASS)
		{
			moveField(o, 0, sizeof(double));
		} else if(o->getType()->getName() == FILE_CLASS)
		{
			moveField(o, 0, sizeof(FILE *));
		}  else if(o->getType()->getName() == STRING_CLASS)
		{
			moveField(o, 1, o->getValue(0));
		} else
		{
			for(unsigned int i = 0; i < o->getFieldCount(); i ++)
			{
				o->setValue(i, moveObject(o->getValue(i)));
			}
		}
	}
	std::swap(used, empty);
}

uint32_t Memory::moveObject(uint32_t ptr)
{
	if(ptr == VM_NULL)
	{
		return VM_NULL;
	}
	Object * o = (Object *)(used + ptr);
	if(o->address == VM_NULL)
	{
		//copy object
		memcpy(empty + size, used + ptr, objectSize);
		//set forward pointer
		o->address = size;
		moved.push(size);
		size += objectSize;
		Object * copy = (Object *) (empty + o->address);
		//alocate fields
		copy->setFields((uint32_t *)(empty + size));
		size += o->getFieldCount() * sizeof(uint32_t);
		//copy field values
		memcpy(copy->getFields(), o->getFields(), o->getFieldCount() * sizeof(uint32_t));
	}
	return o->address;
}

bool Memory::isSpace(uint32_t requested) const
{
	return requested + size < maxSize;
}

void Memory::moveField(Object * o, uint32_t field, uint32_t fieldSize)
{
	if(o->getValue(field) == VM_NULL)
	{
		return;
	}
	memcpy(empty + size, used + o->getValue(field), fieldSize);
	o->setValue(field, size);
	size += fieldSize;
}


