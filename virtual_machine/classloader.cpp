#include "classloader.h"
#include <inttypes.h>
#include <iostream>
#include "bytecodeconstants.h"

using namespace std;

ClassLoader::ClassLoader(const char * folder): folder(folder), file(), classTable()
{
	Class * integer = new Class(INT_CLASS);
	integer->setFieldCount(1);
	classTable[INT_CLASS] = integer;
	Class * real = new Class(REAL_CLASS);
	real->setFieldCount(1);
	classTable[REAL_CLASS] = real;
	Class * string = new Class(STRING_CLASS);
	string->setFieldCount(2);
	classTable[STRING_CLASS] = string;
	classTable[ARRAY_CLASS] = new Class(ARRAY_CLASS);
	classTable[FILE_CLASS] = new Class(FILE_CLASS);
}

ClassLoader::~ClassLoader()
{
	for(map<string, Class *>::iterator it = classTable.begin(); it != classTable.end(); it++)
	{
		delete it->second;
	}
	classTable.clear();
}

Class * ClassLoader::getClass(const char * className)
{
	if(classTable.find(className) == classTable.end())
	{
		Class * c = loadClass(className);
		classTable[className] = c;
	}
	return classTable[className];
}

Class * ClassLoader::loadClass(const char * className)
{
	uint16_t id;
	uint8_t fieldCount, methodCount;
	char name[IDENTIFIER_LENGTH];
	Class * cl;
	file.open(getFileName(className).c_str(),  ios::in | ios::binary);
	if(!file.good())
	{
		throw "Class not found!";
	}
	file.read((char *)&id, sizeof(uint16_t));
	file.read(name, IDENTIFIER_LENGTH);
	cl = new Class(name);
	cl->setConstPool(loadConstantPool());
	file.read((char *)&fieldCount, sizeof(uint8_t));
	cl->setFieldCount(fieldCount);
	cout << "field count = " << (int)fieldCount << endl;
	file.read((char *)&methodCount, sizeof(uint8_t));
	cout << "method count = " << (int)methodCount << endl;
	for(uint8_t i = 0; i < methodCount; i ++)
	{
		cl->addMethod(loadMethod());
	}
	file.close();
	cout << "id = " << id << "\nname = " << cl->getName() << endl;
	return cl;
}


Method * ClassLoader::loadMethod()
{
	char name[IDENTIFIER_LENGTH];
	uint8_t flag, paramCount, localCount;
	uint16_t codeSize;
	char * code;
	file.read(name, IDENTIFIER_LENGTH);
	Method * m = new Method(name);
	file.read((char *)&flag, sizeof(uint8_t));
	m->setFlag(flag);
	file.read((char *)&paramCount, sizeof(uint8_t));
	m->setParamCount(paramCount);
	file.read((char *)&localCount, sizeof(uint8_t));
	m->setParamCount(localCount);
	file.read((char *)&codeSize, sizeof(uint16_t));
	code = new char[codeSize];
	file.read(code, codeSize);
	m->setCode(code, codeSize);
	delete [] code;
	cout << m->debugString() << endl;
	return m;
}

ConstantPool * ClassLoader::loadConstantPool()
{
	uint16_t poolSize;
	ConstantPool * pool = new ConstantPool();
	ConstPoolStruct * item;
	file.read((char *)&poolSize, sizeof(uint16_t));
	cout << "const pool size = " << poolSize << endl;
	for(uint16_t i = 0; i < poolSize; i ++)
	{
		uint8_t type;
		file.read((char *)&type, sizeof(uint8_t));
		switch(type)
		{
			case METHOD_REF:
			{
				item = new MethodRef;
				file.read((char *)item, sizeof(MethodRef));
				cout << "MethodRef[\"" << ((MethodRef *)item)->name << "\", " << (int)((MethodRef *)item)->params << "]" << endl;
				break;
			}
			case CLASS_REF:
			{
				item = new ClassRef;
				file.read((char *)item, sizeof(ClassRef));
				cout << "ClassRef[\""  << ((ClassRef *)item)->name << "\"]" << endl;
				break;
			}
			case STRING_CONST:
			{
				StringConst * s = new StringConst;
				item = s;
				file.read((char *)&s->length, sizeof(uint16_t));
				s->value = new char[s->length];
				file.read(s->value, s->length);
				cout << "String[\""  << s->value << "\", " << s->length << "]" << endl;
				break;
			}
			case INT_CONST:
			{
				item = new IntConst;
				file.read((char *)item, sizeof(IntConst));
				cout << "Int[" << ((IntConst *)item)->value << "]" << endl;
				break;
			}
			case REAL_CONST:
			{
				item = new RealConst;
				file.read((char *)item, sizeof(RealConst));
				cout << "Real[" << ((RealConst *)item)->value << "]" << endl;
				break;
			}
			default:
				throw 10;
		}
		pool->addItem(item, (ConstPoolTag) type);
	}
	return pool;
}



string ClassLoader::getFileName(const char * className)
{
	string filename = folder;
	filename.append("/").append(className).append(".lessie");
	return filename;
}
