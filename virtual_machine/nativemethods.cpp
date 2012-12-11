#include "method.h"
#include "object.h"
#include "memory.h"
#include <iostream>
#include <string>
#include <string.h>

class Print: public NativeMethod
{
public:
	Print(): NativeMethod("print")
	{
		setParamCount(1);
	}
	
	uint32_t run(Memory * heap)
	{
		if(params[0] == VM_NULL)
		{
			std::cout << "null";
			return VM_NULL;
		}
		Object * o = heap->getObject(params[0]);
		std::string className = o->getType()->getName();
		if(className == INT_CLASS)
		{
			std::cout << o->getValue(0);
		} else if(className == REAL_CLASS)
		{
			std::cout << heap->getDoubleValue(o->getValue(0));
		} else if(className == STRING_CLASS)
		{
			std::cout << heap->getStringValue(o->getValue(1));
		}
		return VM_NULL;
	}
};

class Println: public NativeMethod
{
public:
	Println(): NativeMethod("println")
	{
		setParamCount(1);
	}
	
	uint32_t run(Memory * heap)
	{
		if(params[0] == VM_NULL)
		{
			std::cout << "null" << std::endl;
			return VM_NULL;
		}
		Object * o = heap->getObject(params[0]);
		std::string className = o->getType()->getName();
		if(className == INT_CLASS)
		{
			std::cout << o->getValue(0) << std::endl;
		} else if(className == REAL_CLASS)
		{
			std::cout << heap->getDoubleValue(o->getValue(0)) << std::endl;
		} else if(className == STRING_CLASS)
		{
			std::cout << heap->getStringValue(o->getValue(1)) << std::endl;
		}
		return VM_NULL;
	}
};

class ReadLine: public NativeMethod
{
public:
	ReadLine(Class * str): NativeMethod("readLine"), str(str)
	{
		setParamCount(0);
	}
	
	uint32_t run(Memory * heap)
	{
		std::string buffer;
		getline(std::cin, buffer);
		return heap->allocateString(str, buffer.c_str());
	}
	
private:
	Class * str;
};

class ReadInt: public NativeMethod
{
public:
	ReadInt(Class * integer): NativeMethod("readInt"), integer(integer)
	{
		setParamCount(0);
	}
	
	uint32_t run(Memory * heap)
	{
		int32_t val;
		std::cin >> val;
		if(std::cin.good())
		{
			return heap->allocateNumber(integer, val);
		}
		return VM_NULL;
	}
	
private:
	Class * integer;
};

class ReadReal: public NativeMethod
{
public:
	ReadReal(Class * real): NativeMethod("readReal"), real(real)
	{
		setParamCount(0);
	}
	
	uint32_t run(Memory * heap)
	{
		double val;
		std::cin >> val;
		if(std::cin.good())
		{
			return heap->allocateNumber(real, val);
		}
		return VM_NULL;
	}
	
private:
	Class * real;
};

class FileOpen: public NativeMethod
{
public:
	FileOpen(): NativeMethod("open")
	{
		setParamCount(3);
	}
	
	uint32_t run(Memory * heap)
	{
		Object * self = heap->getObject(params[0]);
		Object * fileName = heap->getObject(params[1]);
		Object * mask = heap->getObject(params[2]);
		if(self->getType()->getName() != FILE_CLASS || fileName->getType()->getName() != STRING_CLASS)
		{
			throw "Invalid parameter type for file open.";
		}
		uint32_t file = heap->createFile(heap->getStringValue(fileName->getValue(1)), heap->getStringValue(mask->getValue(1)));
		self->setValue(0, file);
		return VM_NULL;
	}
	
};

class FileClose: public NativeMethod
{
public:
	FileClose(): NativeMethod("close")
	{
		setParamCount(1);
	}
	
	uint32_t run(Memory * heap)
	{
		Object * self = heap->getObject(params[0]);
		if(self->getType()->getName() != FILE_CLASS)
		{
			throw "Not a file.";
		}
		FILE * file = heap->getFileStream(self->getValue(0));
		fclose(file);
		return VM_NULL;
	}
};

class FileReadLine: public NativeMethod
{
public:
	FileReadLine(Class * str): NativeMethod("readLine"), str(str)
	{
		setParamCount(1);
	}
	
	uint32_t run(Memory * heap)
	{
		Object * self = heap->getObject(params[0]);
		if(self->getType()->getName() != FILE_CLASS)
		{
			throw "Not a file.";
		}
		FILE * file = heap->getFileStream(self->getValue(0));
		std::string line;
		char buffer[256];
		while(true)
		{
			fgets(buffer, 256, file);
			unsigned int len = strlen(buffer);
			if(buffer[len - 1] == '\n')
			{
				buffer[len - 1] = 0;
				len --;
			}
			line.append(buffer);
			if(len < sizeof(buffer) - 1)
			{
				break;
			}
		}
		return heap->allocateString(str, line.c_str());
	}
private:
	Class * str;
};

class FileReadInt: public NativeMethod
{
public:
	FileReadInt(Class * integer): NativeMethod("readInt"), integer(integer)
	{
		setParamCount(1);
	}
	
	uint32_t run(Memory * heap)
	{
		Object * self = heap->getObject(params[0]);
		if(self->getType()->getName() != FILE_CLASS)
		{
			throw "Not a file.";
		}
		FILE * file = heap->getFileStream(self->getValue(0));
		int32_t value;
		int i  = fscanf(file, "%i", &value);
		int err = ferror(file);
		return heap->allocateNumber(integer, value);
	}
private:
	Class * integer;
};

class FileReadReal: public NativeMethod
{
public:
	FileReadReal(Class * real): NativeMethod("readReal"), real(real)
	{
		setParamCount(1);
	}
	
	uint32_t run(Memory * heap)
	{
		Object * self = heap->getObject(params[0]);
		if(self->getType()->getName() != FILE_CLASS)
		{
			throw "Not a file.";
		}
		FILE * file = heap->getFileStream(self->getValue(0));
		double value;
		fscanf(file, "%lf", &value);
		return heap->allocateNumber(real, value);
	}
private:
	Class * real;
};

class FileWrite: public NativeMethod
{
public:
	FileWrite(): NativeMethod("write")
	{
		setParamCount(2);
	}
	
	uint32_t run(Memory * heap)
	{
		Object * self = heap->getObject(params[0]);
		if(self->getType()->getName() != FILE_CLASS)
		{
			throw "Not a file.";
		}
		FILE * file = heap->getFileStream(self->getValue(0));
		Object * o = heap->getObject(params[1]);
		std::string className = o->getType()->getName();
		if(className == INT_CLASS)
		{
			fprintf(file, "%d", o->getValue(0));
		} else if(className == REAL_CLASS)
		{
			fprintf(file, "%lf", heap->getDoubleValue(o->getValue(0)));
		} else if(className == STRING_CLASS)
		{
			fprintf(file, "%s", heap->getStringValue(o->getValue(1)));
		}
		return VM_NULL;
	}
};

class IsNull: public NativeMethod
{
public:
	IsNull(Class * integer): NativeMethod("isNull"), integer(integer)
	{
		setParamCount(1);
	}
	
	uint32_t run(Memory * heap)
	{
		if(params[0] == VM_NULL)
		{
			return heap->allocateNumber(integer, 1);
		} else
		{
			return heap->allocateNumber(integer, 0);
		}
	}
private:
	Class * integer;
};

class ArraySize: public NativeMethod
{
public:
	ArraySize(Class * integer): NativeMethod("size"), integer(integer)
	{
		setParamCount(1);
	}
	
	uint32_t run(Memory * heap)
	{
		Object * self = heap->getObject(params[0]);
		if(self->getType()->getName() != ARRAY_CLASS)
		{
			throw "Not an array.";
		}
		return heap->allocateNumber(integer, (int32_t)self->getFieldCount());
	}
private:
	Class * integer;
};

