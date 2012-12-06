#include "memory.h"
#include "bytecodeconstants.h"
#include <string.h>

#define MEMORY_SIZE 1024
#define FIELDS_SIZE 10000
#define FILES_SIZE 1000

#define STRING_SPACE(x) (x)/sizeof(uint32_t) + ((x) % sizeof(uint32_t) == 0 ? 0 : 1)

using namespace std;

Memory::Memory(): memory(new Object[MEMORY_SIZE], new Object[MEMORY_SIZE], MEMORY_SIZE), 
	fields(new uint32_t[FIELDS_SIZE], new uint32_t[FIELDS_SIZE], FIELDS_SIZE)
{
}

Memory::~Memory()
{
	ints.clear();
}

uint32_t Memory::allocate(Class * cls)
{
	Object o(cls);
	uint32_t fieldCount = o.getFieldCount();
	if(!fields.isSpace(fieldCount) || !memory.isSpace())
	{
		return VM_NULL;
	}
	if(fieldCount > 0)
	{
		o.setFields(fields.used + fields.size);
		fields.size += fieldCount;
	}
	memory.used[memory.size] = o;
	return memory.size ++;
}

Object * Memory::getObject(uint32_t pointer) const
{
	if(pointer >= memory.size)
	{
		throw "Memory violation.";
	}
	return &(memory.used[pointer]);
}

uint32_t Memory::allocateNumber(Class * cls, double value)
{
	static unsigned int doubleSize = sizeof(double)/sizeof(uint32_t);
	if(!fields.isSpace(doubleSize))
	{
		return VM_NULL;
	}
	uint32_t ptr = allocate(cls);
	if(ptr == VM_NULL)
	{
		return ptr;
	}
	Object * o = getObject(ptr);
	memcpy(fields.used + fields.size, &value, sizeof(double));
	o->setValue(0, fields.size);
	fields.size += doubleSize;
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
	//TODO intern strings
	unsigned int length = strlen(value);
	unsigned int memSize = STRING_SPACE(length);
	if(!fields.isSpace(memSize))
	{
		return VM_NULL;
	}
	uint32_t ptr = allocate(cls);
	if(ptr ==VM_NULL)
	{
		return VM_NULL;
	}
	Object * str = getObject(ptr);
	memcpy(fields.used + fields.size, value, length);
	str->setValue(0, length);
	str->setValue(1, fields.size);
	fields.size += memSize;
	return ptr;
}

uint32_t Memory::allocateArray(Class * cls, uint32_t length)
{
	if(!fields.isSpace(length) || !memory.isSpace())
	{
		return VM_NULL;
	}
	Object o(cls, length);
	if(length > 0)
	{
		o.setFields(fields.used + fields.size);
		fields.size += length;
	}
	memory.used[memory.size] = o;
	return memory.size ++;
}

const char * Memory::getStringValue(uint32_t ref) const
{
	if(ref >= fields.size)
	{
		throw "Memory violation.";
	}
	return (char *)(fields.used + ref);
}

double Memory::getDoubleValue(uint32_t ref) const
{
	if(ref >= fields.size)
	{
		throw "Memory violation.";
	}
	return *(double *)(fields.used + ref);
}

FILE * Memory::getFileStream(uint32_t file)
{
	if(file >= fields.size)
	{
		throw "No such file descriptor.";
	}
	return (FILE *)(fields.used + file);
}

uint32_t Memory::createFile(const char * fileName)
{
	FILE * file = fopen(fileName,  "rw");
	if(file)
	{
		memcpy(fields.used + fields.size, &file, sizeof(FILE *));
		uint32_t ret = fields.size;
		fields.size += sizeof(FILE *);
		return ret;
	} else 
	{
		return VM_NULL;
	}
}

void Memory::gc(StackFrame * stack)
{
	StackFrame * sf = stack;
	memory.size = 0;
	fields.size = 0;
	ints.clear();
	while(sf)
	{
		for(unsigned int i = 0; i < sf->size(); i ++)
		{
			sf->setValueAt(i, (sf->getValueAt(i)));
		}
		sf = sf->getParent();
	}
	unsigned int scan = 0;
	while(scan < memory.size)
	{
		Object * o = &(memory.empty[scan]);
		if(o->getType()->getName() == INT_CLASS)
		{
			ints[(int)o->getValue(0)] = scan;
		} else if(o->getType()->getName() == REAL_CLASS)
		{
			static unsigned int doubleSize = sizeof(double)/sizeof(uint32_t);
			memcpy(fields.empty + fields.size, fields.used + o->getValue(0), sizeof(double));
			o->setValue(0, fields.size);
			fields.size += doubleSize;
		} else if(o->getType()->getName() == FILE_CLASS)
		{
			memcpy(fields.empty + fields.size, fields.used + o->getValue(0), sizeof(FILE *));
			o->setValue(0, fields.size);
			fields.size += sizeof(FILE *);
		}  else if(o->getType()->getName() == STRING_CLASS)
		{
			memcpy(fields.empty + fields.size, fields.used + o->getValue(1), o->getValue(0));
			o->setValue(1, fields.size);
			fields.size += STRING_SPACE(o->getValue(0));
		} else
		{
			for(unsigned int i = 0; i < o->getFieldCount(); i ++)
			{
				o->setValue(i, moveObject(o->getValue(i)));
			}
		}
		scan ++;
	}
	memory.swap();
	fields.swap();
}

uint32_t Memory::moveObject(uint32_t ptr)
{
	if(memory.used[ptr].address == VM_NULL)
	{
		//copy object
		memory.empty[memory.size] = memory.used[ptr];
		//alocate fields
		memory.empty[memory.size].setFields(fields.empty + fields.size);
		fields.size += memory.used[ptr].getFieldCount();
		//copy field values
		memcpy(memory.empty[memory.size].getFields(), memory.used[ptr].getFields(), memory.used[ptr].getFieldCount() * sizeof(uint32_t));
		//set forward pointer
		memory.used[ptr].address = memory.size ++;
	}
	return memory.used[ptr].address;
}
