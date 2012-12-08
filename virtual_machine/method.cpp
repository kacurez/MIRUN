#include "method.h"
#include <string.h>
#include <sstream>

Method::Method(const char * name): name(name), code(0), flag(0), locals(0)
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

void Method::setLocals(uint8_t locals)
{
	this->locals = locals;
}

uint8_t Method::getLocals() const
{
	return locals;
}

std::string Method::debugString() const
{
	std::stringstream ss;
	ss << "Method[" << name << "," << (int) paramCount << "," << (int)locals << "," << (int)flag << "]";
	return ss.str();
}

void NativeMethod::setParam(uint8_t param, uint32_t value)
{
	if(param >= paramCount)
	{
		throw "No such parameter.";
	}
	params[param] = value;
}

NativeMethod::~NativeMethod()
{
	delete params;
}

NativeMethod::NativeMethod(const char * name): Method(name), params(0)
{
	flag = NATIVE_METHOD;
}

void NativeMethod::setParamCount(uint8_t val)
{
	Method::setParamCount(val);
	params = new uint32_t[paramCount];
}

uint8_t Method::getFlag() const
{
	return flag;
}

bool Method::isNative() const
{
	return (flag & 0x02);
}

uint16_t Method::getCodeSize() const
{
	return codeSize;
}
