/**
 * @author Jakub Valenta
 */
#ifndef CONSTANTPOOL_H
#define CONSTANTPOOL_H

#include <inttypes.h>
#include <vector>
#include "bytecodeconstants.h"

struct ConstPoolStruct {};

struct MethodRef: public ConstPoolStruct
{
	char name[IDENTIFIER_LENGTH];
	uint8_t params;
};

struct ClassRef: public ConstPoolStruct
{
	char name[IDENTIFIER_LENGTH];
};

struct StringConst: public ConstPoolStruct
{
	uint16_t length;
	char * value;
};

struct IntConst: public ConstPoolStruct
{
	uint32_t value;
};

struct RealConst: public ConstPoolStruct
{
	double value;
};

class ConstPoolItem 
{
public:
	ConstPoolItem(ConstPoolTag type, ConstPoolStruct * data);
	~ConstPoolItem();
	ConstPoolTag getType() const;
	int getIntValue() const;
	double getRealValue() const;
	const char * getStringValue() const;
	uint8_t getParam() const;
	
private:
	ConstPoolTag  type;
	int intValue;
	double realValue;
	char * string;
	uint8_t param;
};

class ConstantPool
{
public:
	ConstantPool();
	~ConstantPool();
	void addItem(ConstPoolStruct * item, ConstPoolTag type);
	ConstPoolItem * getItem(unsigned int i) const;

private:
	std::vector<ConstPoolItem *> pool;
};

#endif // CONSTANTPOOL_H
