#include "classloader.h"
#include <inttypes.h>
#include <iostream>
#include "bytecodeconstants.h"
#include "nativemethods.cpp"

using namespace std;

ClassLoader::ClassLoader(const char * folder): folder(folder), file(), classTable()
{
	initBuiltInClasses();
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
	cout << "field count = " << (int)fieldCount << endl;
	for(uint8_t i = 0; i< fieldCount; i ++)
	{
		file.read(name, IDENTIFIER_LENGTH);
		cl->addField(name);
	}
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
				throw "Invalid class file.";
		}
		pool->addItem(item, (ConstPoolTag) type);
		if(type == STRING_CONST)
		{
			delete [] (((StringConst *) item)->value);
		}
		delete item;
	}
	return pool;
}



string ClassLoader::getFileName(const char * className)
{
	string filename = folder;
	filename.append("/").append(className).append(".lessie");
	return filename;
}

void ClassLoader::addClass(Class * c)
{
	classTable[c->getName()] = c;
}

void ClassLoader::initBuiltInClasses()
{
	classTable[INT_CLASS] = new Class(INT_CLASS);
	classTable[INT_CLASS]->addField("value");

	classTable[REAL_CLASS] = new Class(REAL_CLASS);
	classTable[REAL_CLASS]->addField("value");
	
	classTable[STRING_CLASS] = new Class(STRING_CLASS);
	classTable[STRING_CLASS]->addField("length");
	classTable[STRING_CLASS]->addField("value");
	
	classTable[ARRAY_CLASS] = new Class(ARRAY_CLASS);
	
	classTable[FILE_CLASS] = new Class(FILE_CLASS);
	classTable[FILE_CLASS]->addField("file");
	classTable[FILE_CLASS]->addMethod(new FileOpen());
	classTable[FILE_CLASS]->addMethod(new FileClose());
	classTable[FILE_CLASS]->addMethod(new FileWrite());
	classTable[FILE_CLASS]->addMethod(new FileReadLine(classTable[STRING_CLASS]));
	classTable[FILE_CLASS]->addMethod(new FileReadInt(classTable[INT_CLASS]));
	classTable[FILE_CLASS]->addMethod(new FileReadReal(classTable[REAL_CLASS]));
	
	classTable[CONSOLE_CLASS] = new Class(CONSOLE_CLASS);
	classTable[CONSOLE_CLASS]->addMethod(new Print());
	classTable[CONSOLE_CLASS]->addMethod(new ReadLine(classTable[STRING_CLASS]));
	classTable[CONSOLE_CLASS]->addMethod(new ReadInt(classTable[INT_CLASS]));
	classTable[CONSOLE_CLASS]->addMethod(new ReadReal(classTable[REAL_CLASS]));
}
