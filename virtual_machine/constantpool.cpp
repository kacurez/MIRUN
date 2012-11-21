#include "constantpool.h"

ConstantPool::ConstantPool()
{
	//ctor
}

ConstantPool::~ConstantPool()
{
	for(unsigned int i = 0; i < pool.size(); i++)
	{
		ConstPoolStruct * s = pool[i];
		if(poolType[i] == STRING_CONST)
		{
			delete [] ((StringConst *) s)->value;
		}
		delete s;
	}
	pool.clear();
	poolType.clear();
}

void ConstantPool::addItem(ConstPoolStruct * item, ConstPoolTag type)
{
	pool.push_back(item);
	poolType.push_back(type);
}
