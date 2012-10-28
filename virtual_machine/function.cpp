#include "function.h"
#include <string.h>

Function::Function(const char * name): name(name), code(0)
{
	
}

Function::~Function()
{
	delete [] code;
}

void Function::setParamCount(uint8_t val)
{
	paramCount = val;
}

uint8_t Function::getParamCount() const
{
	return paramCount;
}

void Function::setCode(const unsigned char * val, uint16_t size, uint32_t offset)
{
	delete [] code;
	codeSize = size;
	code = new unsigned char[size];
	memcpy(code, val + offset, sizeof(unsigned char) * size);
}

const unsigned char * Function::getCode() const
{
	return code;
}

std::string Function::getName() const
{
	return name;
}

