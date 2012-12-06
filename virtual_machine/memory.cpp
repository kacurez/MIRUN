#include "memory.h"
#include "bytecodeconstants.h"
#include <string.h>

#define MEMORY_SIZE 5
#define FIELDS_SIZE 100000

using namespace std;

Memory::Memory(): memory(new Object[MEMORY_SIZE], new Object[MEMORY_SIZE], MEMORY_SIZE), 
	fields(new uint32_t[FIELDS_SIZE], new uint32_t[FIELDS_SIZE], FIELDS_SIZE), 
	doubles(new double[MEMORY_SIZE], new double[MEMORY_SIZE], MEMORY_SIZE)
{
}

Memory::~Memory()
{
	ints.clear();
	for(vector<char *>::iterator it = strings.begin(); it < strings.end(); it ++)
	{
		delete [] (*it);
	}
	strings.clear();
	for(vector<fstream *>::iterator it = files.begin(); it < files.end(); it ++)
	{
		(*it)->close();
		delete [] (*it);
	}
	files.clear();
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
	if(!doubles.isSpace())
	{
		return VM_NULL;
	}
	uint32_t ptr = allocate(cls);
	if(ptr ==VM_NULL)
	{
		return ptr;
	}
	Object * o = getObject(ptr);
	doubles.used[doubles.size] = value;
	o->setValue(0, doubles.size);
	doubles.size ++;
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
	uint32_t ptr = allocate(cls);
	if(ptr ==VM_NULL)
	{
		return ptr;
	}
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
	if(ref >= strings.size())
	{
		throw "Memory violation.";
	}
	return strings[ref];
}

double Memory::getDoubleValue(uint32_t ref) const
{
	if(ref >= doubles.size)
	{
		throw "Memory violation.";
	}
	return doubles.used[ref];
}

fstream * Memory::getFileStream(uint32_t file)
{
	if(file >= doubles.size)
	{
		throw "No such file descriptor.";
	}
	return files[file];
}

uint32_t Memory::createFile(const char * fileName)
{
	fstream * file = new fstream;
	file->open(fileName,  ios::in | ios::out | ios::binary);
	if(file->is_open())
	{
		files.push_back(file);
		return files.size() - 1;
	} else 
	{
		delete file;
		return VM_NULL;
	}
}

void Memory::gc(StackFrame * stack)
{
	StackFrame * sf = stack;
	memory.size = 0;
	fields.size = 0;
	doubles.size = 0;
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
			doubles.empty[doubles.size] = doubles.used[o->getValue(0)];
			o->setValue(0, doubles.size);
			doubles.size ++;
		} else if(o->getType()->getName() == STRING_CLASS)
		{
			
		} else
		{
			for(unsigned int i = 0; i < o->getFieldCount(); i ++)
			{
				o->setValue(i, moveObject(o->getValue(i)));
			}
		}
		scan ++;
	}
	std::swap(memory.used, memory.empty);
	std::swap(fields.used, fields.empty);
	std::swap(doubles.used, doubles.empty);
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
