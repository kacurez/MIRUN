#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>
#include <inttypes.h>

#define CLASS_METHOD 0
#define INSTANCE_METHOD 1
#define NATIVE_METHOD 2

class Method
{
public:
	Method(const char * name);
	virtual ~Method();
	std::string getName() const;
	const char * getCode() const;
	void setCode(const char * val, uint16_t size, uint32_t offset = 0);
	uint8_t getParamCount() const;
	virtual void setParamCount(uint8_t val);
	void setFlag(uint8_t flag);
	void setLocals(uint8_t locals);
	uint8_t getLocals() const;
	std::string debugString() const;
	uint8_t getFlag() const;
	bool isNative() const;
	
protected:
	std::string name;
	char * code;
	uint16_t codeSize;
	uint8_t paramCount;
	uint8_t flag;
	uint8_t locals;
};

class Memory;

class NativeMethod: public Method
{
public:
	NativeMethod(const char * name);
	~NativeMethod();
	void setParam(uint8_t param, uint32_t value);
	virtual uint32_t run(Memory * heap) = 0;
	void setParamCount(uint8_t val);

protected:
	uint32_t * params;
	
};

#endif // FUNCTION_H
