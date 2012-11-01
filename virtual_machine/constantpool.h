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
	uint16_t id;
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

class ConstantPool
{
public:
	ConstantPool();
	~ConstantPool();
	void addItem(ConstPoolStruct * item, ConstPoolTag type);

private:
	std::vector<ConstPoolStruct *> pool;
	std::vector<ConstPoolTag> poolType;
};

#endif // CONSTANTPOOL_H
