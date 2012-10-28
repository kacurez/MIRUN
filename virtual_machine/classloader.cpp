#include "classloader.h"
#include <inttypes.h>
#include <iostream>

#define IDENTIFIER_LENGTH 25
using namespace std;

ClassLoader::ClassLoader(): file()
{
}

ClassLoader::~ClassLoader()
{
}

void ClassLoader::load(const char * filename)
{
	uint16_t classCount;
	file.open(filename,  ios::in | ios::binary);
	file.read((char *)&classCount, sizeof(uint16_t));
	cout << "class count = " << classCount << endl;
	while(file.peek() != EOF)
	{
		loadClass();
	}
	file.close();
}

struct MethodRef
{
	char name[IDENTIFIER_LENGTH];
	uint8_t params;
};

struct ClassRef
{
	char name[IDENTIFIER_LENGTH];
	uint16_t id;
};

struct StringConst
{
	uint16_t length;
	char * value;
};

typedef uint32_t IntConst;

typedef double RealConst;

void ClassLoader::loadClass()
{
	uint16_t id, poolSize;
	char name[IDENTIFIER_LENGTH];
	Class * cl;
	file.read((char *)&id, sizeof(uint16_t));
	file.read(name, IDENTIFIER_LENGTH);
	cl = new Class(name);
	file.read((char *)&poolSize, sizeof(uint16_t));
	for(uint16_t i = 0; i < poolSize; i ++)
	{
		uint8_t type;
		file.read((char *)&type, sizeof(uint8_t));
		switch(type)
		{
			case 1:
				MethodRef m;
				file.read((char *)&m, sizeof(MethodRef));
				cout << "MethodRef[\"" << m.name << "\", " << m.params << "]" << endl;
				break;
			case 2:
				ClassRef c;
				file.read((char *)&c, sizeof(ClassRef));
				cout << "ClassRef[\"" << c.name << "\", " << c.id << "]" << endl;
				break;
			case 3:
				StringConst s;
				file.read((char *)&s.length, sizeof(uint16_t));
				s.value = new char[s.length];
				file.read(s.value, s.length);
				cout << "String[\"" << s.value << "\", " << s.length << "]" << endl;
				break;
			case 4:
				IntConst i;
				file.read((char *)&i, sizeof(IntConst));
				cout << "Int[" << i << "]" << endl;
				break;
			case 5:
				RealConst r;
				file.read((char *)&r, sizeof(RealConst));
				cout << "Real[" << r << "]" << endl;
				break;
			default:
				throw 10;
		}
	}
	cout << "id = " << id << "\nname = " << cl->getName() << endl;
	delete cl;
}
