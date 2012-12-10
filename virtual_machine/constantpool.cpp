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

uint16_t ConstantPool::addItem(ConstPoolStruct * item, ConstPoolTag type)
{
	ConstPoolItem * i = new ConstPoolItem(type, item);
	pool.push_back(i);
	return pool.size() - 1;
}

ConstPoolItem* ConstantPool::getItem(unsigned int i) const
{
	return pool[i];
}

uint16_t ConstantPool::size() const
{
	return pool.size();
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
			intValue = s->length;
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
		case FIELD_REF:
		{
			FieldRef * f = (FieldRef *) data;
			string = new char[IDENTIFIER_LENGTH];
			memcpy(string, f->name, IDENTIFIER_LENGTH);
			break;
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

