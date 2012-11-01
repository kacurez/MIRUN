#include "method.h"
#include <string.h>

Method::Method(const char * name): name(name), code(0), flag(0)
{
	
}

Method::~Method()
{
	delete [] code;
}

void Method::setParamCount(uint8_t val)
{
	paramCount = val;
}

uint8_t Method::getParamCount() const
{
	return paramCount;
}

void Method::setCode(const char * val, uint16_t size, uint32_t offset)
{
	delete [] code;
	codeSize = size;
	code = new char[size];
	memcpy(code, val + offset, size);
}

const char * Method::getCode() const
{
	return code;
}

std::string Method::getName() const
{
	return name;
}

void Method::setFlag(uint8_t flag)
{
	this->flag = flag;
}


