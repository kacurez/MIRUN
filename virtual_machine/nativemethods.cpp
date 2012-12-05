#include "method.h"
#include "object.h"
#include "memory.h"
#include <iostream>
#include <string>

class Print: public NativeMethod
{
public:
	Print(): NativeMethod("print")
	{
		setParamCount(1);
	}
	
	uint32_t run(Memory * heap)
	{
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
			std::cout << heap->getStringValue(o->getValue(0)) << std::endl;
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
		return heap->allocateNumber(integer, val);
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
		return heap->allocateNumber(real, val);
	}
	
private:
	Class * real;
};

class FileOpen: public NativeMethod
{
public:
	FileOpen(): NativeMethod("open")
	{
		setParamCount(2);
	}
	
	uint32_t run(Memory * heap)
	{
		Object * self = heap->getObject(params[0]);
		Object * fileName = heap->getObject(params[0]);
		if(self->getType()->getName() != FILE_CLASS || fileName->getType()->getName() != STRING_CLASS)
		{
			throw "Invalid parameter type for file open.";
		}
		uint32_t file = heap->createFile(heap->getStringValue(fileName->getValue(1)));
		self->setValue(0, file);
		return file;
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
		std::fstream * file = heap->getFileStream(self->getValue(0));
		file->close();
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
		std::fstream * file = heap->getFileStream(self->getValue(0));
		std::string buffer;
		getline(*file, buffer);
		return heap->allocateString(str, buffer.c_str());
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
		std::fstream * file = heap->getFileStream(self->getValue(0));
		int32_t value;
		(*file) >> value;
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
		std::fstream * file = heap->getFileStream(self->getValue(0));
		double value;
		(*file) >> value;
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
		std::fstream * file = heap->getFileStream(self->getValue(0));
		Object * o = heap->getObject(params[0]);
		std::string className = o->getType()->getName();
		if(className == INT_CLASS)
		{
			(* file) << o->getValue(0);
		} else if(className == REAL_CLASS)
		{
			(* file) << heap->getDoubleValue(o->getValue(0));
		} else if(className == STRING_CLASS)
		{
			(* file) << heap->getStringValue(o->getValue(0));
		}
		return VM_NULL;
	}
};
