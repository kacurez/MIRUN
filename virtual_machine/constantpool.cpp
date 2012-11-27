#include "constantpool.h"
#include <string.h>

using namespace std;

ConstantPool::ConstantPool()
{
	//ctor
}

ConstantPool::~ConstantPool()
{
	for(vector<ConstPoolItem *>::iterator it = pool.begin(); it < pool.end(); it ++)
	{
		delete *it;
	}
	pool.clear();
}

void ConstantPool::addItem(ConstPoolStruct * item, ConstPoolTag type)
{
	ConstPoolItem * i = new ConstPoolItem(type, item);
	pool.push_back(i);
}

ConstPoolItem* ConstantPool::getItem(unsigned int i) const
{
	return pool[i];
}

ConstPoolItem::ConstPoolItem(ConstPoolTag type, ConstPoolStruct * data): type(type), string(0)
{
	switch(type)
	{
		case METHOD_REF:
		{
			MethodRef * m = (MethodRef *) data;
			param = m->params;
			string = new char[IDENTIFIER_LENGTH];
			memcpy(string, m->name, IDENTIFIER_LENGTH);
			break;
		}
		case CLASS_REF:
		{
			ClassRef * c = (ClassRef *) data;
			string = new char[IDENTIFIER_LENGTH];
			memcpy(string, c->name, IDENTIFIER_LENGTH);
			break;
		}
		case STRING_CONST:
		{
			StringConst * s = (StringConst *) data;
			string = new char[s->length];
			memcpy(string, s->value, s->length);
			break;
		}
		case INT_CONST:
		{
			intValue = ((IntConst *) data)->value;
		}
		case REAL_CONST:
		{
			realValue = ((RealConst *) data)->value;
		}
	}
}

uint8_t ConstPoolItem::getParam() const
{
	return param;
}

const char * ConstPoolItem::getStringValue() const
{
	return string;
}

double ConstPoolItem::getRealValue() const
{
	return realValue;
}

int ConstPoolItem::getIntValue() const
{
	return intValue;
}

ConstPoolTag ConstPoolItem::getType() const
{
	return type;
}

 ConstPoolItem::~ConstPoolItem()
{
	delete [] string;
}

